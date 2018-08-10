<img style="width:100%;" src="images/github-banner.jpeg">

# RebirthDB


[![Build Status](https://travis-ci.org/RebirthDB/rebirthdb.svg?branch=next)](https://travis-ci.org/RebirthDB/rebirthdb)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/e486b20dee3141c89dcb974fe1ae16de)](https://www.codacy.com/app/RebirthDB/rebirthdb?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=RebirthDB/rebirthdb&amp;utm_campaign=Badge_Grade) [![Coverage Status](https://coveralls.io/repos/github/RebirthDB/rebirthdb/badge.svg?branch=next)](https://coveralls.io/github/RebirthDB/rebirthdb?branch=next)

## What is RebirthDB?

* **Open-source** database for building realtime web applications
* **NoSQL** database that stores schemaless JSON documents
* **Distributed** database that is easy to scale
* **High availability** database with automatic failover and robust fault tolerance

RebirthDB is a community-developed fork of [RethinkDB](https://github.com/rethinkdb/rethinkdb) which was the first 
open-source scalable database built for realtime applications. It exposes a new database access model -- instead of
polling for changes, the developer can tell the database to continuously push updated query results to applications 
in realtime. RebirthDB allows developers to build scalable realtime apps in a fraction of the time with less effort.


## Quick start

### 1. Install the server

To install RebirthDB on your machine (only the `amd64` / `x86_64` architecture is currently supported), run the following:

#### **Ubuntu** `Trusty`, `Xenial`, `Bionic` and **Debian** `Jessie`, `Stretch` versions

```bash
$ source /etc/lsb-release && echo "deb https://dl.bintray.com/rebirthdb/apt $DISTRIB_CODENAME main" | sudo tee /etc/apt/sources.list.d/rebirthdb.list

$ wget -qO- https://dl.bintray.com/rebirthdb/keys/pubkey.gpg | sudo apt-key add -

$ sudo apt-get update

$ sudo apt-get install rebirthdb
```

#### **Centos 7**
```bash
sudo wget https://dl.bintray.com/rebirthdb/rpm/centos/7/x86_64/rebirthdb.repo \
    -O /etc/yum.repos.d/rebirthdb.repo
sudo yum install rebirthdb 
```

### 2. Start the server
Run the following command from your terminal:
```bash
$ rebirthdb
...
Listening for intracluster connections on port 29015
Listening for client driver connections on port 28015
Listening for administrative HTTP connections on port 8080
Listening on cluster addresses: 127.0.0.1, ::1
Listening on driver addresses: 127.0.0.1, ::1
Listening on http addresses: 127.0.0.1, ::1
...
Server ready ...
```

Point your browser to localhost:8080. You’ll see an administrative UI where you can control the cluster 
(which so far consists of one server), and play with the query language.

### 3. Run some queries

Click on the Data Explorer tab in the browser. You can manipulate data using JavaScript straight from your browser.
By default, RebirthDB creates a database named `test`. Let’s create a table:

```javascript
r.db('test').tableCreate('tv_shows')
```

Use the “Run” button or Shift+Enter to run the query. Now, let’s insert some JSON documents into the table:

```javascript
r.table('tv_shows').insert([{ name: 'Star Trek TNG', episodes: 178 },
                            { name: 'Battlestar Galactica', episodes: 75 }])
```

We’ve just inserted two rows into the tv_shows table. Let’s verify the number of rows inserted:

```javascript
r.table('tv_shows').count()
```

Finally, let’s do a slightly more sophisticated query. Let’s find all shows with more than 100 episodes.

```javascript
r.table('tv_shows').filter(r.row('episodes').gt(100))
```
As a result, we of course get the best science fiction show in existence.

### Next steps

Congratulations on your progress! Now check out the documentation of any of our drivers below to dive deeper.

* **Elixir** - [rebirthdb-elixir](https://github.com/RebirthDB/rebirthdb-elixir)
* **Go** - [rebirthdb-go](https://github.com/RebirthDB/rebirthdb-go)
* **Haskell** - [rebirthdb-haskell](https://github.com/RebirthDB/rebirthdb-haskell)
* **Java** - [rebirthdb-java](https://github.com/RebirthDB/rebirthdb-java)
* **JavaScript** - [rebirthdb-js](https://github.com/RebirthDB/rebirthdb-js)
* **Lua** - [rebirthdb-lua](https://github.com/RebirthDB/rebirthdb-lua)
* **PHP** - [rebirthdb-php](https://github.com/RebirthDB/rebirthdb-php)
* **Python** - [rebirthdb-python](https://github.com/RebirthDB/rebirthdb-python)
* **Ruby** - [rebirthdb-ruby](https://github.com/RebirthDB/rebirthdb-ruby)
* **Rust** - [rebirthdb-rs](https://github.com/RebirthDB/rebirthdb-rs)
* **TypeScript** - [rebirthdb-ts](https://github.com/RebirthDB/rebirthdb-ts)

More detailed and updated documentation is coming soon!

## Building

First install some dependencies.  For example, on Ubuntu or Debian:

    sudo apt-get install build-essential protobuf-compiler python \
        libprotobuf-dev libcurl4-openssl-dev libboost-all-dev \
        libncurses5-dev libjemalloc-dev libssl-dev wget m4 g++

Generally, you will need

* GCC or Clang
* Protocol Buffers
* jemalloc
* Ncurses
* Boost
* Python 2
* libcurl
* libcrypto (OpenSSL)
* libssl-dev

Then, to build:

    ./configure --allow-fetch
    # or run ./configure --allow-fetch CXX=clang++

    make -j4
    # or run make -j4 DEBUG=1

    sudo make install
    # or run ./build/debug_clang/rebirthdb


## Need help?

Find us at [Spectrum](https://spectrum.chat/rebirthdb) and on [Twitter](https://twitter.com/rebirthdb). You can also
[report an issue](https://github.com/rebirthdb/rebirthdb/issues).


## Contributing
 
RebirthDB is currently being developed by a growing and passionate community. We could use your help too!
Check out our [contributing guidelines](CONTRIBUTING.md) to get started.


## Where's the changelog?

We keep [a list of changes and feature explanations here](NOTES.md).


## Donors

* The development of data compression and RebirthDB's new art is sponsored by [AIDAX](http://www.aidaxbi.com/):<br>
[![AIDAX](images/sponsors/AIDAX_logo_whole.png)](http://www.aidaxbi.com/)

* Our test infrastructure is sponsored by [DigitalOcean](https://www.digitalocean.com/):<br>
<a href="https://www.digitalocean.com/"> <img src="https://opensource.nyc3.digitaloceanspaces.com/attribution/assets/SVG/DO_Logo_horizontal_blue.svg" width="500px"></a>
