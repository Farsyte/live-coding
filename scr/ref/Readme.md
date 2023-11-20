# Reference Documents

These are intended to be as much of the text of my printed
reference documents as I am willing to hand-type and proof; I am
likely to leave out machine dependent items that I do not yet
need.

The symbol ... may appear where details have been elided.



## MODEL: fig-FORTH MODEL

The "MODEL.scr" was hand-entered from my printed copy of:

    fig-FORTH INSTALLATION MANUAL
    GLOSSARY MODEL EDITOR
    RELEASE 1 WITH COMPILER SECURITY AND VARIABLE LENGTH NAMES
    BY WILLIAM F. RAGSDALE
    November 1980

Much content has been elided. This file is intended to be used
in conjunction with the printed text (or a PDF of it).

Note that some bits are elided with "..." and must be
constructed for your target machine.

Note that some bits are marked IMPLDEP, review these when tuning
the code for your IMPL.

## Forward References

I am interested in working out if it is possible to produce a Forth
that does not need forward references, aside from vectors stored in
a vector table at a fixed location early in the image.

I think this can be discovered from the model by extracting tuples of
the form "word A references word B" and using "tsort" to detect
circular dependencies and extract a potential ordering.

Where a high level word definition uses an IMMEDIATE word, the
dependency is not on the immedaite word, but on the references
compiled into the target image by that word.

Another way to check this is to attempt to construct FORTH in
sections, carefully starting with code that has no dependencies, then
adding sections that depend only on what has come before.

This may lead to collecting most assembly code together near the start
of the image, rather than allowing chunks in ";CODE" sections of
otherwise portable defining words.

