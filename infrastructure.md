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
