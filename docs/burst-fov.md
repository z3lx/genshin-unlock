# Optional burst FOV framing fix

The fix is **disabled by default**. Add these fields to `plugin_config.json`
(alongside your existing settings) to enable it:

```json
"fixBurstFov": true,
"burstFovDelayMs": 1600,
"burstFovKey": "Q"
```

`unlockFov` must also be enabled. Set `fixBurstFov` to `false` to use the
original FOV handling without the burst heuristic. The normal cursor/focus,
menu restoration and hook-deactivation logic is not replaced.

`burstFovKey` must match your in-game Elemental Burst key (default `Q`).
The workaround only triggers when the camera takeover happens within
600 ms of pressing it, so transformations and gadgets that take over the
camera the same way (Chenyu Vale fish, Snezhnaya hooks, etc.) keep your
selected FOV. Pressing the key while the burst is unavailable does nothing,
since no camera takeover follows. Bursts cast with a controller are not
detected and use the original FOV handling.

`burstFovDelayMs` is a whole number from **0 to 10000 milliseconds**, with a
**1600 ms** default. It caps how long native FOV is preserved after the
caller-pattern detector triggers. Return begins sooner if the other learned
camera caller reappears. Delay changes apply to the next detected burst.

The return uses a fixed **200 ms smooth blend**, independent of `fovSmoothing`.
Increasing the delay by 100 delays the timed return by 100 ms; it does not
make the blend slower. For a timer-limited burst, 1600 ms + 200 ms means the
selected FOV is reached approximately 1800 ms after detection.

Existing configuration files are upgraded the same way as for any other new
setting: on first load the plugin keeps your current values and rewrites the
file with the new fields set to their defaults. Invalid values are still
rejected.

This is a heuristic, not an exact animation-end signal. It has been tested
by the reporter on several owned characters, not the full roster or every
camera state. Too short a delay can expose cinematic overlay edges again.
The key is read from the plugin's existing keyboard state polling (the same
one used for the FOV toggle and preset keys). There are no new hooks, memory
reads, screen capture or diagnostic logging.
Preserving the existing hook lifecycle is not a guarantee of account safety.

## Why the burst key is required

The detector watches which game code sets the camera FOV. In normal
gameplay two call sites alternate on the same camera; when a burst starts,
only one of them keeps calling. That pattern alone turned out not to be
specific to bursts: transformations and gadgets that take over the camera
(Chenyu Vale fish, Snezhnaya hooks, etc.) produce exactly the same switch,
so the caller pattern alone would force native FOV for them too.

Combining the two signals fixes this. A key press alone cannot trigger
anything, because a burst that is unavailable (no energy, on cooldown)
causes no camera switch. A camera switch alone cannot trigger anything
either, because transformations are not started with the burst key. If a
switch happens without a recent key press, the detector disarms and must
see the normal alternating pattern again before it can trigger, so pressing
the key later while transformed does not start the bypass. Each key press
can start at most one bypass.
