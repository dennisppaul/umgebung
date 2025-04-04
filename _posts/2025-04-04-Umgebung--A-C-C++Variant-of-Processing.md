---
layout: post
title:  "Umgebung — A C/C++ Variant of Processing "
date:   2025-04-04 22:00:00 +0100
---

![2025-04-04-Umgebung--A-C-C++Variant-of-Processing.png](/assets/2025-04-04-Umgebung--A-C-C++Variant-of-Processing.png)

hej community,

i have been (very) busy the last weeks making a point: ever since i started using [Processing](https://processing.org) as an environment or paradigm for _designing with code_, i wanted to make a C/C++ variant of it. finally, i did:

> _Umgebung_ is a lightweight C++ environment for small audio- and graphics-based applications. it is inspired by [Processing.org](https://processing.org) and similar environments.

i’m now sharing this with you because i think the environment, the approach, but first and foremost the _attitude_ and _aura_ could be interesting, relevant, and helpful for you. i’ve tried to outline a few reasons why i think that is ( a bit more on the technical side ) in the [Why _Umgebung_?](https://github.com/dennisppaul/umgebung) text. but there is much more to say about this … i truly think that it has a political tangent … maybe more on that at a later point ;)

so, if you’re looking for a DM-style weekend project, why not head over to the GitHub repository of _Umgebung_ and give it a try:

https://github.com/dennisppaul/umgebung

⚠️⚠️⚠️ DISCLAIMER ⚠️⚠️⚠️

this is still very unfinished and experimental software. there’s a good chance that a lot of things do not work. if you’re on windows, it’s likely that you cannot use it at all ( at this point ). but since you shouldn’t be on windows anyway, maybe this is a good time to finally install Ubuntu ;)

AAMOF, i’ve tested _Umgebung_ on a MacBook Pro M3, a Mac Mini (intel-based) — both running macOS 15.3 — on that same Mac Mini running Ubuntu 24.04 LTS, and on a Raspberry Pi 4 Model B running _Bookworm_.

i’ve created a [Quick Start](https://github.com/dennisppaul/umgebung) guide ( which i’m also using to test on different machines ), and if you’re really into it, i also created an image that should set up your RPi 4B or 5 just like that:

http://dm-hb.de/umgebung-rpi ( it’s a bit untidy ;) )

there are a few other documentation documents and examples distributed all over the repository. it’s all very beta still.

also, the environment is next-level nerd and peak-geek ( which is part of the _aura_ and attitude i talked about earlier ). it helps if you have a basic understanding of the Command Line Interface (CLI), or at least it assumes you’re not afraid of it ( some DM people are not afraid 👋 @jiawen 👋 ).

BTW, if you _are_ afraid, don’t worry — but also question _why_ that is? what are you afraid of? who told you to be afraid?

the easiest way to control people and keep them in check and “normal” is by making them _afraid_, and that’s especially true for technology in general and programming in particular. so, don’t be _afraid_, but more importantly, know that you are not alone! we are a community, and there are people who want to — and will — help!!!

**coding is caring!**

this is why i set up a discord server where we can share and talk ( yet another debatable platform, but i wanted to open this up to non-HfK people … convenience > æthics ):

https://d3-is.de/umgebung

apart from me being excited to share this with you, i also want to ask for your help and support — in whatever way you deem to be feasible: suggesting new features, discussing the political implications, requesting library integrations, finding bugs, writing examples, or just sharing code snippets, and so on. i would also really, _really_ love it if someone contributed to the _actual_ code base ( one can dream, one can dream ).

this is not _my_ project — it never was ( i obviously ripped the whole idiom from [Processing](https://processing.org) ;) ) — and the sooner others contribute to make it a communal effort, the happier i am ;)

furthermore, i plan to introduce this environment to the bachelor students in an upcoming class. this is DEFINITELY an experiment that could go multiple ways. so, the more this thing has been tested and used … the better, i guess.

so again, take a look at it, let me know what you think ( including critical and non-confirming points ), and if this project doesn’t get you excited, your :heart: might need a firmware update!

PS { just to give you a bit of a frame of reference for what can already be done with _Umgebung_: Leo Puhl @leonat wrote his excellent bachelor thesis with it … try multi-channel sound processing like _that_ in java- or web-based processing ;) }

PPS { and since you’re obviously still reading, you might also find this interview as interesting as i did, in regards to _attitude_: https://open.spotify.com/episode/2qfg3lPaetR4mp580btkOW?si=7b22c2812a94465c — but this is a bit OT }

PPPS { i am also trying to occasionally write a developer blog at https://umgebung.dennisppaul.de }