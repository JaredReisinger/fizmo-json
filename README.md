# fizmo-json

A [Fizmo](https://github.com/chrender/fizmo) front-end meant for
bot-mediated streaming/flowing UIs, like
[xyzzybot](https://github.com/JaredReisinger/xyzzybot).


## Usage





## Background

In working on [xyzzybot](https://github.com/JaredReisinger/xyzzybot), I noticed
a conflict between the expectations that a Slack-based interface presented and
the affordances given by [RemGlk](https://github.com/erkyrath/remglk) and
[fizmo-remglk](https://github.com/chrender/fizmo-remglk).  In particular, while
the Glk API seems ideal for "local" game interactions, it adds a lot of
complexity to the "remote" scenario.  Further, Fizmo has its own engine/client
API, which means that it’s ripte for a scenario-specific front-end.
