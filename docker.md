Docker Notes
============

Getting PostgreSQL Up & Running
-------------------------------

So I don't tear my hair out again figuring out how to make stuff work, here are the steps
I've used to get a postgresql database up and running with Docker, initialised with a
dump of our Trac database.

Getting our Trac database is fairly straight-forward with `pg_dump`.

We need to create a Dockerfile with the contents along the lines of:

```
FROM postgres:alpine

ADD . /docker-entrypoint-initdb.d
```

Copy our database here, call it something like `trac.db.sql`, and we'll also add a user as well:

```
#!/bin/bash
set -e

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" <<-EOSQL
    CREATE USER trac;
    CREATE DATABASE trac;
    GRANT ALL PRIVILEGES ON DATABASE trac TO trac;
EOSQL
```

I'm not yet sure if adding the trac user is actually required or not, haven't gotten that far.

Anyway, we can build with `sudo docker build .`.

Once we start it up, and init has finished, we can verify everything is running by connecting
with `psql`, where $CONTAINER is the container we've started:

```
sudo docker run -it --rm --link $CONTAINER:postgres postgres:alpine psql -h postgres -U postgres
```

Can use commands like `\list` and `\dt` to confirm our database is configured correctly, and run
some queries on the listed tables to ensure the data looks correct.

TODO: Use a separate data volume for the database files.

Next Steps
----------

Next up is building a container for Trac, and getting it to communicate with our postgres
container. We'll probably want to have a separate container for the web server as well. Nginx
or Apache2? I assume Trac can run under Nginx. It uses WSGI or whatever it is with Apache2...

