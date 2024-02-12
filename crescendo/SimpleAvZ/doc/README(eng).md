# Simple AvZ 💫

[English](./README(eng).md) | [简体中文](../README.md)

*---------- the brand-new AvZ experience*

Simple AvZ is a syntax simplification framework based on AvZ 1.

## Quick Start

Use AvZ VSCode extension's `Get AvZ Extension` command to install Simple AvZ, and then add `#include "SimpleAvZ/lib.h"` to your script. That's it!

If `Get AvZ Extension` is broken, you may also manually download it from the `/release` directory in this repo.

## API (v1.0.0)

### Setting Wave

```cpp
waves(1, 2, 3);             // w1, w2, w3 (may specify any number of waves)
waves({1, 9}, 4);           // from w1 to w9, every 4 waves
waves({1, 9}, {11, 19}, 4); // from w1 to w9 and from w11 to w19, every 4 waves
```

```cpp
for (auto w : waves(...)) {
    // here goes your operations...
    // you may use if statements for conditional operations
    if (w == 1) {
        C(359, DOOM, 2, 9);
    } else if (w == 5) {
        C(359, DOOM, 3, 9);
    }
}
```

All Simple AvZ functions, unless marked with `[EXT]`, shall be used within such for loops.

### Setting Time

```cpp
(318, ...)            // take effect at 318cs of the current wave
(after(110), ...)     // take effect 110cs after the last operation
(now, ...)            // "now" is equivalent to "after(0)"
```

Most Simple AvZ functions' first parameter is time. `after()` accepts negative values, but doing so is not recommended as it hampers readability.


### Using Cob

```cpp
PP(318);               // Fire (2,9) and (5,9), taking effect at 318cs
PP(318, 8);            // Fire (2,8) and (5,8)
PP(318, {8, 9});       // Fire (2,8) and (5,9)
PP(318, {2, 6}, 9);    // Fire (2,9) and (6,9)
```

Fire two cobs. If rows are omitted, they default to row 2 & 5 for PE/FE and row 2 & 4 for other scenes. If col is omitted, it defaults to 9.

```cpp
PP(after(110), ...);   // Take effect after 110cs
PP(now, ...);          // "now" is equivalent to "after(0)"
```

Alternative forms of time. Applicable to other functions as well.

```cpp
DD(...);               // Same usage as PP
```

Fire two interception cobs. If rows are omitted, they default to row 1 & 5 for PE/FE and row 1 & 4 for other scenes. If col is omitted, it defaults to 9.

```cpp
P(318, 2, 9);              // Fire (2,9), taking effect at 318cs
B(...);                    // Separate cob, same usage as P
D(...);                    // Interception cob, same usage as P
P(318, 1, 1, 2, 8);        // Use cob at 1-1 to fire (2,8)
```

Fire one cob. You may specify which cob to use. You may use `P`, `B`, or `D` to indicate your intent (there is no actual difference).

```cpp
ExcludeCob(3, 5);     // Do not use cob at 3-5, effective since game start [EXT]
ExcludeCob(400, ...); // Effective since 400cs of the current wave
ResetCob(400);        // Reset to use all cobs, effective since 400cs of the current wave
```

Exclude certain cobs, or reset to use all cobs.

```cpp
CobOperator c1(1);       // Only use cobs with tail at col 1
CobOperator c45(4, 5);   // Only use cobs with tail at col 4 or 5

void Script() {
    c1.PP(...);     // Same usage
}
```

In roof scenes, you may declare multiple `CobOperator`s before `void Script()`, and then call the above cob functions in the same way.

### Using Cards / Shovel

> Ice, doom, cherry, jala, squash use cob-based time (don't worry if you don't understand what this means).

```cpp
I(1, 2);             // Place ice at 1-2, taking effect at 601cs (perfect ice)
I(after(210), 1, 2); // Use ice, taking effect after 210cs (ice3), recommended to be used after activation cobs
I(359, 1, 2);        // Use ice, taking effect at 359cs
M_I(...);            // Use imitater ice (same usage)
```

Use ice at nighttime. Effect time is auto-corrected. If effect time is not specified, it defaults to 601cs of the current wave (perfect ice for next wave).

```cpp
SetIce({{1, 1}, {1, 2}});  // Store and use ices at 1-1, 1-2 (use 1-2 first), effective since game start [EXT]
SetIce(400, {...});        // Effective since 400cs
```

Set where to store ices at daytime. If effect time is not specified, it defaults to game start (wave 1, -599cs).

```cpp
I();                       // Use ice, taking effect at 601cs (perfect ice)
I(after(210));             // Use ice, taking effect after 210cs (ice3), recommended to be used after activation cobs
I(359);                    // Use ice, taking effect at 359cs
```

Use ice at daytime. Effect time is auto-corrected. If effect time is not specified, it defaults to 601cs of the current wave (perfect ice for next wave).

```cpp
C(359, CHERRY, 2, 9);                // Place cherry at 2-9, taking effect at 359cs
C(400, {PUFF, SUN}, {1, 2}, 9);      // Place puff-shroom at 1-9 and sun-shroom at 2-9 
C(359, {LILY, DOOM, COFFEE}, 3, 9);  // Place lily pad, doom, and coffee bean at 3-9
```

Use cards. You may provide a single position, multiple rows with the same column, or multiple cards at the same position.

```cpp
C(after(110), ...);            // Same usage, taking effect after 110cs
C(exact(800), ...);            // Use card-based time instead of cob-based time, taking effect at 800cs
C(exact(after(..)), ...);      // Combination of the above two
```

Alternative forms of card times.

```cpp
C(400, keep(266), ...);        // Shovel 266cs after effect time
C(400, until(1036), ...);      // Shovel at 1036cs
```

Specify the shovel time after using cards.

```cpp
RM(400, SUNFLOWER);            // Shovel all sunflowers at 400cs
RM(400, PUMPKIN, 1, 1);        // Shovel pumpkin at 1-1 (if there is no pumpkin, do nothing)
RM(400, 1, 1);                 // Shovel 1-1 (non-pumpkin plants first)
RM(400, {1, 2, 5, 6}, 9);      // Shovel 1-9, 2-9, 5-9, 6-9 (non-pumpkin plants first)
RM(after(751), ...);           // Same usage, taking effect after 751cs
```

Shovel plants. You may provide a single position, multiple rows with the same column, or a certain plant type to be shoveled.

### Demo Aid

```cpp
EnsureExist(GIGA);                           // Ensure giga appears in all reasonable rows
EnsureExist({GIGA, 2, 3});                   // Ensure giga appears in row 2 & 3
EnsureExist({{GIGA, 2, 3}, {ZOMBONI, 4}});   // Ensure giga appears in row 2 & 3 and zomboni appears in row 4
```

Ensure some zombies appear in certain rows for the current wave. Mostly used for demonstration purposes.

Please DO NOT use this in actual gameplay or batch tests.

```cpp
EnsureAbsent({GIGA, 2, 3});                   // Ensure giga are absent in row 2 & 3
EnsureAbsent({{GIGA, 2, 3}, {ZOMBONI, 4}});   // Ensure giga are absent in row 2 & 3 and zomboni are absent in row 4
```

Ensure some zombies are absent in certain rows for the current wave. Mostly used for demonstration purposes.

Please DO NOT use this in actual gameplay or batch tests.

### Smart Card

```cpp
C_IF(exist(ZOMBONI), 400, SPIKEWEED, 1, 9);     // If there is zomboni in row 1, place spikeweed at 1-9 at 400cs
C_IF(pos({GARG, GIGA}, 680), 400, POT, 1, 8);   // If there is garg or giga with int(x)≤680, place flower pot at 1-8 at 400cs
```

Decide whether or not to use cards depending on zombies. You may either check if a certain type of zombie exists, or check if certain types of zombie's x is smaller than some threshold. Mostly used in actual gameplay or batch tests.

For demonstration purposes, using `EnsureExist` is more recommended as it showcases the worst case scenario.

## Script Examples

Here are some [Simple AvZ script examples](../examples/).

## Change Log

View [Simple AvZ's change log](./CHANGELOG.md).

## Learn More

Learn more about [Simple AvZ's rationale behind the scenes](./About.md) (Chinese).
