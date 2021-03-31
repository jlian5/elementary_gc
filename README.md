# Elementary GC
Garbage collector for SP21 CS296-41 project

# TODO LIST

(sorted bottom up)

## Memory compaction
- [ ] Implement memory compaction for free objects

## Generational GC
- [ ] Add age to object metadata
- [ ] Create separate lists for each age group
- [ ] Run garbage collector at different intervals for each group

## Use of GC
- [x] Create basic test file that uses malloc, calloc, realloc, free
- [ ] Run mark and sweep collector every `malloc`

## GC Algorithm Implementation
- [ ] Basic object allocation + graph creation
- [x] Mark and sweep algorithm skeleton
- [x] Mark and sweep algorithm code

## Initial Setup
- [x] Setup makefile
- [x] Add vector files from MP (jlian5, 2/18)
  - I(jlian5) added my own vector files i coded in my mp, if any of yall know how to add it like how they handled it in mp_shell you can try.

