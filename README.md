<img style="width:100%;" src="images/github-banner.jpeg">

RebirthDB
=========

[![Build Status](https://travis-ci.org/RebirthDB/rebirthdb.svg?branch=next)](https://travis-ci.org/RebirthDB/rebirthdb)
[![Coverage Status](https://coveralls.io/repos/github/RebirthDB/rebirthdb/badge.svg?branch=next)](https://coveralls.io/github/RebirthDB/rebirthdb?branch=next)

What is RebirthDB?
------------------

* **Open-source** database for building realtime web applications
* **NoSQL** database that stores schemaless JSON documents
* **Distributed** database that is easy to scale
* **High availability** database with automatic failover and robust fault tolerance

RebirthDB is a community-developed fork of [RethinkDB](https://github.com/rethinkdb/rethinkdb) which was the first 
open-source scalable database built for realtime applications. It exposes a new database access model -- instead of
polling for changes, the developer can tell the database to continuously push updated query results to applications 
in realtime. RebirthDB allows developers to build scalable realtime apps in a fraction of the time with less effort.

To learn more, check out [rethinkdb.com](https://rethinkdb.com).

Not sure what types of projects RethinkDB and now RebirthDB can help you build? Here are a few examples:

* Build a [realtime liveblog](https://rethinkdb.com/blog/rethinkdb-pubnub/) with RethinkDB and PubNub
* Create a [collaborative photo sharing whiteboard](https://www.youtube.com/watch?v=pdPRp3UxL_s)
* Build an [IRC bot in Go](https://rethinkdb.com/blog/go-irc-bot/) with RethinkDB changefeeds
* Look at [cats on Instagram in realtime](https://rethinkdb.com/blog/cats-of-instagram/)
* Watch [how Platzi uses RethinkDB](https://www.youtube.com/watch?v=Nb_UzRYDB40) to educate


Quickstart
----------

For a thirty-second RethinkDB quickstart (which can easily be applied to RebirthDB), check out  [rethinkdb.com/docs/quickstart](https://www.rethinkdb.com/docs/quickstart).

Or, get started right away with the RethinkDB ten-minute guide in these languages:

* [**JavaScript**](https://rethinkdb.com/docs/guide/javascript/)
* [**Python**](https://rethinkdb.com/docs/guide/python/)
* [**Ruby**](https://rethinkdb.com/docs/guide/ruby/)
* [**Java**](https://rethinkdb.com/docs/guide/java/)

Here are RebirthDB's drivers:

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

Looking to explore what else RethinkDB offers or the specifics of ReQL? Check out [the RethinkDB docs](https://rethinkdb.com/docs/) and [ReQL API](https://rethinkdb.com/api/).

Building
--------

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


Need help?
----------

Find us at [Spectrum](https://spectrum.chat/rebirthdb) and on [Twitter](https://twitter.com/rebirthdb). You can also
[report an issue](https://github.com/rebirthdb/rebirthdb/issues).

Contributing
------------

RethinkDB was built by a dedicated team, but it wouldn't have been possible without the support and contributions of 
hundreds of people from all over the world. 
 
RebirthDB is currently being developed by a growing and dedicated community. We could use your help too!
Check out our [contributing guidelines](CONTRIBUTING.md) to get started.

Where's the changelog?
----------------------
We keep [a list of changes and feature explanations here](NOTES.md).

Donors
------
* The development of data compression and RebirthDB's new art is sponsored by [AIDAX](http://www.aidaxbi.com/):<br>
[![AIDAX](images/sponsors/AIDAX_logo_whole.png)](http://www.aidaxbi.com/)

* Our test infrastructure is sponsored by [DigitalOcean](https://www.digitalocean.com/):<br>
<a href="https://www.digitalocean.com/"> <img src="https://opensource.nyc3.digitaloceanspaces.com/attribution/assets/SVG/DO_Logo_horizontal_blue.svg" width="500px"></a>
