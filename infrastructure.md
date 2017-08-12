Haiku Infrastructure
====================

Introduction
------------

These are my instructions on building container images for all parts of Haiku's
infrastructure. They will aim to be as complete as possible, so that our
infra is well documented.

Trac Installation
-----------------

We need to use `pg_dump` to get a copy of the current database that we can use
to seed our database for our container, call it `trac.db.sql`.

With our database dump in hand, we can create our database container for trac.

First, we need a data volume:
```
docker run -v /var/lib/postgresql/data --name trac-db-data busybox /bin/true
```

Now our database container...

Dockerfile:
```
FROM postgres:alpine
MAINTAINER Haiku, Inc.

ENV POSTGRES_USER trac

ADD trac.db.sql /docker-entrypoint-initdb.d/trac.db.sql
```

And run to get us our database:
```
docker build .
# use -e POSTGRES_PASSWORD=somepassword to configure a password
docker run --volumes-from trac-db-data --name trac-db <hash>
# CTRL+C once done, then start it in the background
docker start trac-db
```

Check that our database contains our data:
```
docker run -it --rm --link trac-db:postgres postgres:alpine psql -h postgres -U trac
# commands to verify everything is working:
\c trac
\dt
SELECT COUNT(*) FROM ticket;
\q
```

cgit Installation
-------------------

We are going to need to store the git repositories somewhere, and likely
share this across containers, so let's make a data volume for that first:
```
docker run -v /repositories --name repositories busybox /bin/true
```

I'm not sure if this will double as our master repository, it may be better
for that to be separate. Anyway, for now, we can clone our repositories
into here with a temporary alpine linux image:
```
docker run --volumes-from repositories --rm -it alpine sh
apk add --no-cache git
git clone --bare https://git.haiku-os.org/haiku /repositories/haiku
git clone --bare https://git.haiku-os.org/buildtools /repositories/buildtools
exit
```

We need a version of Caddy that supports the cgi module, so we'll build a container with
the `cgi` plugin enabled:
```
docker build --build-arg plugins=http.cgi github.com/abiosoft/caddy-docker --tag caddy-cgi:latest
```

Now we can build our `cgit` docker container!

We need a few files for this:
- Dockerfile
- Caddyfile
- cgitrc
- haiku-filter-commit-links.sh
- static files

Our `Dockerfile` builds on top of our `caddy-cgi` image, and is fairly
straight-forward:
```
FROM caddy-cgi

RUN apk add --no-cache cgit

COPY cgitrc /etc/cgitrc
COPY Caddyfile /etc/Caddyfile
COPY haiku-filter-commit-links.sh /usr/lib/cgit/filters/haiku-filter-commit-links.sh
COPY static/* /srv/static
```

Our `Caddyfile`:
```
cgit.haiku.nz
tls jessica.l.hamilton@gmail.com
gzip

rewrite {
	if {path} is /
	to /cgi/{uri}
}

rewrite {
	to {path} /cgi/{uri}
}

cgi {
	match /cgi
	exec /usr/share/webapps/cgit/cgit.cgi
}
```

Our commit links filter:
```
#!/bin/sh
# This script can be used to generate links in commit messages.
#
# To use this script, refer to this file with either the commit-filter or the
# repo.commit-filter options in cgitrc.

# This expression generates links to commits referenced by their SHA1.
regex=$regex'
s|(\s+)([0-9a-fA-F]{7,40})(\s+)|\1<a href="./?id=\2">\2</a>\3|g'

# This expression generates links to commits referenced by their revision tag.
regex=$regex'
s|(hrev[0-9]+)|<a href="./?id=\1">\1</a>|g'

# This expression generates links to Trac issues.
regex=$regex'
s|#([0-9]+)|<a href="http://dev.haiku-os.org/ticket/\1">#\1</a>|g'

sed -re "$regex"
```

And our `cgitrc` file, taken from our existing install:
```
clone-prefix=https://git.haiku-os.org ssh://git.haiku-os.org

css=/static/cgit.css
logo=/static/haiku-logo.png
favicon=/static/favicon.ico

enable-commit-graph=1
enable-index-links=1
enable-log-filecount=1
enable-log-linecount=1
root-title=Haiku's repositories
max-atom-items=25
commit-filter=/usr/lib/cgit/filters/haiku-filter-commit-links.sh
max-stats=year
virtual-root=/

mimetype.css=text/css
mimetype.git=image/git
mimetype.html=text/html
mimetype.jpg=image/jpeg
mimetype.jpeg=image/jpeg
mimetype.pdf=application/pdf
mimetype.png=image/png
mimetype.svg=image/svg+xml

enable-filter-overrides=0

repo.url=haiku
repo.path=/repositories/haiku
repo.desc=Haiku's main repository
repo.owner=haiku-inc.org

repo.url=buildtools
repo.path=/repositories/buildtools
repo.desc=Haiku's buildtools repository
repo.owner=haiku-inc.org
```

Build our new image:
```
docker build . --tag haiku-cgit:latest
```

And now we can run it, with all our bits and pieces in place:
```
docker run -p 80:80 -p 443:443 --volumes-from repositories:ro --name cgit.haiku.nz haiku-cgit:latest
```
