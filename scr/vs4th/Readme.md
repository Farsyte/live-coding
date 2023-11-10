* VoidStar8080 Forth (in fig-FORTH)

** Acknowledgements

First, I want to thank the Forth Interest Group for sparking my
interest, and John Cassady in particular for METAFORTH which opened my
eyes to a number of concepts.

The fact that I still have my paper copies of fig-FORTH 8080, the
fig-FORTH model and glossary, and Cassady's METAFORTH should indicate
how much this continues to resonate, even after four decades as a
software engineer.

** Goals of this subproject

Present FORTH, written in FORTH, which can regenerate itself.

Rougly the same goals as METAFORTH's META-SYS-2.

* Top-Down Breakdown of the subproject

** One or more trivial FORTH applications

Cassady's METAFORTH includes a game called STARS. 

** FORTH, the portable parts in FORTH

** FORTH, the ISA-spaecific parts in ASM

FORTH words that can not be written in FORTH,
and assembly code that is needed that is not forth.

- psh2
- psh1
- next
- (*begin*)
- (*colon*)
- (*constant*)
- (*create*)
- (*do*)
- (*does*)
- (*else*)
- (*if*)
- (*literal*)
- (*loop*)
- (*repeat*)
- (*semicolon*)
- (*then*)
- (*until*)
- (*variable*)
- (*while*)

** CROSS-4TH

Modify to compile into the TARGET area.

** CROSS-ASM

Modify to assemble into the TARGET area.

** CROSS-PREP

Set up appropriate context for the above items
