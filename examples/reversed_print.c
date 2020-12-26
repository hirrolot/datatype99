/*
 * Output:
 * abracadabra
 * 3
 * 2
 * 1
 */

#include <mirwa.h>

#include <stdio.h>

Effect(Print);

EffectHandler(Print, void, print, (const char *s), {
    resume();
    puts(args.s);
});

int main(void) {
    MirwaHandlers mirwa_handlers = {0};
    handle_with(Print);

    performEffect(Print, print, ("1"), {
        performEffect(
            Print, print, ("2"), { performEffect(Print, print, ("3"), { puts("abracadabra"); }); });
    });
}
