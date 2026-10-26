# Finite State Machine Diagram — Tetris

States and transitions match `src/brick_game/tetris/tetris_fsm.c`.

Pause is not a separate state — it is the `GameInfo_t.pause` flag, which
blocks gravity and `Left`/`Right`/`Action`/`Down` input inside
`MOVE_STATE` without changing the machine's state; it is always toggled,
regardless of its current value.

```mermaid
stateDiagram-v2
    [*] --> START_STATE

    START_STATE --> SPAWN_STATE : Start

    SPAWN_STATE --> MOVE_STATE : new piece fits
    SPAWN_STATE --> END_STATE : new piece does not fit (game over)

    MOVE_STATE --> MOVE_STATE : Left / Right / Action (rotate)
    MOVE_STATE --> SHIFT_STATE : timer tick (gravity)
    MOVE_STATE --> UNION_STATE : Down (hard drop)

    SHIFT_STATE --> MOVE_STATE : shift down succeeded
    SHIFT_STATE --> UNION_STATE : shift down blocked

    UNION_STATE --> SPAWN_STATE : piece locked, lines cleared, score added

    END_STATE --> SPAWN_STATE : Start (restart: reset field and score)

    note right of MOVE_STATE
        Pause: game_info.pause is always
        toggled.
    end note
```
