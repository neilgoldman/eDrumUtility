E Drum Utility
============

A VST Plugin to increase the playability of electronic drum kits in certain DAWs or drum samplers. I created this to better use my Alesis DM10X inside of REAPER with NI Studio Drummer. Untested outside of that setup.

You can use the hi-hat pedal control to change which MIDI notes are sent for the hi-hat. (E.g. when the pedal is pressed, striking the hi-hat sends midi note #42, and when it's open midi note #46). Allows up to 7 layers of mapping, with customizeable ranges. Some or all layers can be disabled by setting their 'note' value to -1, or setting their Upper Value to 0. The interface is not very intuitive at the moment.

Also allows you to set a minimum duration of drum hits (some DAWs won't play MIDI notes that are too short). In Reaper, this leads to consistent triggering.

There are parameters to enable learning the hi-hat pedal or cymbal-trigger incoming MIDI notes, but they do not currently do anything. There is also a planned feature to allow "locking" the pedal to a certain tightness, like a real hi hat.

Even though it's quite ugly, I hope you find it helpful.

If you'd like to contribute, or fork your own version, you'll also need to install WDL. I use Oli Larkin's [wdl-ol](https://github.com/olilarkin/wdl-ol). But the vanilla Cockos WDL or Tale WDL should probably work fine (I have not tested these). You'll also need to grab 2 files from the [VST SDK](http://www.steinberg.net/en/company/developer.html). (See the WDL notes for where to place these).

Solutions should exist for VS2013, XCode and CodeBlocks. Only the VST2 plugin project is enabled and tested, but I've left the others in, unloaded, in case you'd like to attempt building it for another plugin format (there are aax, rtas, vst2, vst3, and AU).
