#include "course.h"
#include "student.h"
#include <stdint.h>

struct course
{
    uint16_t code;
    enum subject subject;
    int refcount;
};

struct course *course_create(enum subject s, uint16_t code)
{
    struct course *course = malloc(sizeof(struct course));
    course->code = code;
    course->subject = s;
    course->refcount = 1;
    return course;
}

enum subject course_subject(const struct course *c)
{
    return c->subject;
}

uint16_t course_code(const struct course *c)
{
    return c->code;
}

void course_hold(struct course *c)
{
    c->refcount += 1;
}

void course_release(struct course *c)
{
    c->refcount -= 1;
}

int course_refcount(const struct course *c) {
    return c->refcount;
}
