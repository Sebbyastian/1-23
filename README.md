1-23
====

K&R exercise 1-23, with correct (as far as I can see) handling of
trigraphs and line splices. This file is intended to be an informative
testcase.

Write a program to remove all comments from a C program. Don't forget to
handle quotes strings and character constants properly. C comments don't
nest.

In the days of K&R C, the block style of comment was the only style.

/* This is a comment. */

However, in this day and age, the single-line comment is also
accepted in C.

// This is a comment.

C has line splices, which are backslashes immediately followed by   \
newlines, and these can go anywhere in a C source file. The newline \
following the backslash is not considered the end of a single-line  \
comment.

/\
/\
c\
o\
m\
m\
e\
n\
t... Yes, this is also a comment.

Trigraphs can be used as substitutes for some characters. The
backslash in a line splice or a character escape sequence, for
example, can also be entered as ??/

/??/
/??/
c??/
o??/
m??/
... I'll spare you the trouble, you already know this is a comment.

In fact, line splices of both of these varieties can be mixed and
chained together repetitively, and as long as there is a forward-
facing slash before the chain and an asterisk (or another forward-
facing slash) after the chain, and this isn't in a string literal
or character constant, it's the start of a comment.

/\
??/
\
\
??/
??/
* This, for example, is a comment.*\
??/
\
\
??/
??/
/

/??/
\
??/
??/
\
\
?? But don't forget, if that chain of pain isn't part of a        ??/
   comment, it's not to be stripped! These quotes, '\'', by the   ??/
   way, denote character constants. There are no comments in      ??/
   character constants.

'/* This is not a comment. */'

While we're on the topic of character constants, not every character\
constant ends at the first quote. There's an escape sequence so that\
you can inject \'quotes\' into them like \'this\', and you guessed  \
it, also like ??/'this??/'! Just to add insult to injury, there's a \
trigraph sequence, ??', which denotes the ^ character.

So, '/* this isn't a comment */'??'/* ... but this is a comment */...

"String literals have a similar pattern of escape sequences:        \
You can escape \"these quotes\" like \"this\" and ??/"this??/"..."
Additionally, there are also no comments in string literals.

"So neither \"/*this*/\" nor ??/"/*this*/??/" constitutes a comment"
/* ... but this is a comment. */