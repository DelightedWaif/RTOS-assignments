#include <stdbool.h>
#include <stdint.h>
#include "student.h"
#include "course.h"

struct course
{
    uint16_t code;
    enum subject subject;
    int refcount;
};

struct student
{
    struct student_id id;
    bool grad_student;
    struct course *courses[20];
    uint8_t numCourses;
    uint8_t grades[20];
};

struct student *student_create(struct student_id id, bool grad_student)
{
    struct student *newStudent = malloc(sizeof(struct student));
    newStudent->id = id;
    newStudent->grad_student = grad_student;
    newStudent->numCourses = 0;
    return newStudent;
}

void student_free(struct student *s)
{
    for (int i = 0; i < s->numCourses; i++)
    {
        course_release(s->courses[i]);
    }
    free(s);
}

void student_take(struct student *s, struct course *c, uint8_t grade)
{
    s->courses[s->numCourses] = c;
    course_hold(c);
    s->grades[s->numCourses] = grade;
    s->numCourses += 1;
}

int student_grade(struct student *s, struct course *c)
{
    if (s->numCourses == 0)
    {
        return -1;
    }
    for (int i = s->numCourses - 1; i >= 0; i--)
    {
        if (s->courses[i] == c)
        {
            return s->grades[i];
        }
    }
    return -1;
}

double student_passed_average(const struct student *s)
{
    double sum = 0;
    int count = 0;
    if (s->numCourses == 0)
    {
        return 0;
    }

    for (int i = 0; i < s->numCourses; i++)
    {
        if (s->grad_student)
        {
            if (s->grades[i] >= 65)
            {
                sum += s->grades[i];
                count += 1;
            }
        }
        else if (s->grades[i] >= 50)
        {
            sum += s->grades[i];
            count += 1;
        }
    }

    if (sum == 0)
    {
        return 0;
    }
    return sum / count;
}

bool student_promotable(const struct student *s)
{
    if (s->grad_student)
    {
        bool first_fail = false;
        int num_failed = 0;
        for (int i = 0; i < s->numCourses; i++)
        {
            if (s->grades[i] < 65)
            {
                num_failed += 1;
            }
        }
        if (num_failed > 1) {
            return false;
        }
    }
    else
    {
        double sum = 0;
        for (int i = 0; i < s->numCourses; i++)
        {
            sum += s->grades[i];
        }
        double avg = sum / s->numCourses;
        if (avg < 60)
        {
            return false;
        }
    }
    return true;
}
