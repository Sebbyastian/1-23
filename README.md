1-23
====

K&R exercise 1-23, with correct (as far as I can see) handling of trigraphs and line splices.

Write a program to remove all comments from a C program. Don't forget to handle quotes strings and character constants properly. C comments do not nest.

    /* In the days of K&R C, this block style of comment was the only  *
     * style. However, in this day and age,                            */
    // the single-line style of comment is also accepted in C.

    // C has line splices, which are backslashes immediately followed by\
    newlines, and these can go anywhere in a C source file. The newline \
    following the backslash isn't considered the end of a single-line   \
    comment. IOW, this is all one comment.

    /\
    /\
    c\
    o\
    m\
    m\
    e\
    n\
    t... Yes, this is also a comment.

The trouble doesn't stop there. The backslash in a line splice can be represented using the trigraph "??/", which means:

    /??/
    /??/
    c??/
    o??/
    m??/
    ... I'll spare you the trouble, you already know this is a comment.

In fact, line splices of both of these varieties can be mixed and chained together repetitively, and as long as there's a forward-facing slash before the chain and an asterisk (or another forward-facing slash) after the chain, and this isn't in a string literal or character constant, it's the start of a comment.

But don't forget, if that chain of pain isn't part of a comment, it's not to be stripped!

    "While we're on the topic of string literals, not every string  ??/
     literal ends at the first quote. There's an escape sequence so ??/
     that you can inject \"quotes\" into your string literals like  ??/
     \"this\", and you guessed it, also like ??/"this??/""!

    'Character constants have the same escape sequences, which allow\
     you to inject quotes like \'this\' and ??/'this??/' of course, \
     but be careful because there\'s a trigraph sequence, ??', which\
     denotes the ^ character.??' /* So this isn\'t a comment */, but\
     '??'/* this is a comment */

    Again, ??'/* this isn't a comment */'/* but this is a comment */