#ifndef MIRWA_EFFECT_H
#define MIRWA_EFFECT_H

#include <epilepsy.h>

#define EXPAND(...)       __VA_ARGS__
#define UNPARENTHESISE(x) EXPAND(EXPAND x)

#define resume()                                                                                   \
    *line = __LINE__;                                                                              \
    return;                                                                                        \
    case __LINE__:

#define Effect(...)    EPILEPSY_EVAL(EPILEPSY_OVERLOAD_CALL(v(Effect_), v(__VA_ARGS__)))
#define Effect_1(name) Effect_2(name, { char dummy; })
#define Effect_2(name, state)                                                                      \
    v(struct name##EffectState state; const struct { char dummy; } name##Effect)

#define EffectHandler(effect, return_ty, op, params, ...)                                          \
    struct effect##Effect_##op##_Args {                                                            \
        char dummy;                                                                                \
        UNPARENTHESISE(params);                                                                    \
    };                                                                                             \
                                                                                                   \
    return_ty effect##Effect_##op(                                                                 \
        int line[const restrict static 1], struct effect##EffectState *self,                       \
        struct effect##Effect_##op##_Args args) {                                                  \
        switch (*line) {                                                                           \
        case 0:                                                                                    \
            __VA_ARGS__ *line = -1;                                                                \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    static const char effect_handler##__LINE__

#define performEffect(effect, op, args, ...)                                                       \
    do {                                                                                           \
        MirwaHandlers *stack = &mirwa_handlers;                                                    \
        while (stack->effect_discriminant != &(effect##Effect)) {                                  \
            stack = (MirwaHandlers *)stack->prev;                                                  \
        }                                                                                          \
        int line = 0;                                                                              \
        while (1) {                                                                                \
            effect##Effect##_##op(                                                                 \
                &line, (struct effect##EffectState *)stack->effect_state,                          \
                (struct effect##Effect##_##op##_Args){'\0', UNPARENTHESISE(args)});                \
                                                                                                   \
            if (line == -1) {                                                                      \
                break;                                                                             \
            }                                                                                      \
            __VA_ARGS__                                                                            \
        }                                                                                          \
    } while (0)

typedef struct MirwaHandlers {
    struct MirwaHandlers *prev;
    const void *effect_discriminant;
    void *effect_state;
} MirwaHandlers;

#define handle_with(effect)                                                                        \
    mirwa_handlers = (MirwaHandlers) {                                                             \
        .prev =                                                                                    \
            &(MirwaHandlers){                                                                      \
                .prev = mirwa_handlers.prev,                                                       \
                .effect_discriminant = mirwa_handlers.effect_discriminant,                         \
                .effect_state = mirwa_handlers.effect_state,                                       \
            },                                                                                     \
        .effect_discriminant = &(effect##Effect),                                                  \
        .effect_state = &(struct effect##EffectState){0},                                          \
    }

#endif // MIRWA_EFFECT_H
