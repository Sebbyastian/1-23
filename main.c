/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Sebastian Ramadan
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int c, trigraph = 0;
    enum state { root,
                 possible_comment,
                 single_line_comment,
                 multi_line_comment,
                 possible_comment_end,
                 character_constant,
                 string_literal } state = root;

    size_t old_hi_size, new_hi_size, splice_queue_size = 0, qmark_count = 0;
    unsigned char *splice_queue = NULL;

    for (;;) {
        c = getchar();
        if (c == '\?') {
            if (qmark_count == 2) {
                putchar('\?');
                continue;
            }
            qmark_count++;
            continue;
        }

        if (c < 0) {
            break;
        }

        switch (state) {
            case root:                 printf("%.*s", (int) qmark_count, "\?\?");
                                       switch (c) {
                                           case '=':  
                                           case '#':  /* XXX: Depending upon user options, strip comments from #include <here> */
                                                      break;
                                           case '\'': state = qmark_count == 2 ? root : character_constant;
                                                      break;
                                           case '\"': state = string_literal;
                                                      break;
                                           case '/':  state = qmark_count == 2 ? root : possible_comment;
                                                      goto contin;
                                       }
                                       break;
            case single_line_comment:  switch (c) {
                                           case '/':  if (qmark_count != 2) {
                                                          goto contin;
                                                      }
                                           case '\\': c = getchar();
                                                      if (c != '\n') {
                                                          ungetc(c, stdin);
                                                      }
                                                      goto contin;
                                           case '\n': state = root;
                                                      goto contin;
                                       }
            case multi_line_comment:   if (c == '*') {
                                           state = possible_comment_end;
                                       }
                                       goto contin;
            case possible_comment_end: switch (c) {
                                           case '/':  switch (qmark_count) {
                                                          case 0: state = root;
                                                                  goto contin;
                                                          case 1: state = multi_line_comment;
                                                                  goto contin;
                                                          case 2: break;
                                                      }
                                           case '\\': c = getchar();
                                                      if (c != '\n') {
                                           default:       state = multi_line_comment;
                                                      }
                                           case '*':  goto contin;
                                       }
            case character_constant:   printf("%.*s", (int) qmark_count, "\?\?");
                                       switch (c) {
                                           case '/':  if (qmark_count != 2) {
                                                          break;
                                                      }
                                           case '\\': putchar(c);
                                                      c = getchar();
                                                      break;
                                           case '\'': if (qmark_count != 2) {
                                                          state = root;
                                                      }
                                                      break;
                                       }
                                       break;
            case string_literal:       printf("%.*s", (int) qmark_count, "\?\?");
                                       switch (c) {
                                           case '/':  if (qmark_count != 2) {
                                                          break;
                                                      }
                                           case '\\': putchar(c);
                                                      c = getchar();
                                                      break;
                                           case '\"': if (qmark_count != 2) {
                                                          state = root;
                                                      }
                                                      break;
                                       }
                                       break;
            case possible_comment:     switch (c) {
                                           case '*':  splice_queue_size = 0;
                                                      state = multi_line_comment;
                                                      goto contin;
                                           case '/':  switch (qmark_count) {
                                                          case 0: splice_queue_size = 0;
                                                                  state = single_line_comment;
                                                                  goto contin;
                                                          case 1: goto failure;
                                                          case 2: trigraph = 1;
                                                                  qmark_count = 0;
                                                                  break;
                                                      }
                                           case '\\': if (qmark_count != 0) {
                                                          goto failure;
                                                      }

                                                      c = getchar();
                                                      if (c != '\n') {
                                                          ungetc(c, stdin);
                                                          c = trigraph ? '/' : '\\';
                                                          qmark_count = qmark_count ? qmark_count : 2 * trigraph;
                                                          trigraph = 0;
                                                          goto failure;
                                                      }

                                                      old_hi_size = splice_queue_size / CHAR_BIT;
                                                      new_hi_size = old_hi_size + 1;
                                                      if ((old_hi_size & new_hi_size) == 0) {
                                                          void *temp = realloc(splice_queue, old_hi_size + new_hi_size);
                                                          if (temp == NULL) {
                                                              free(splice_queue);
                                                              perror("realloc");
                                                              return EXIT_FAILURE;
                                                          }
                                                          splice_queue = temp;
                                                          memset(splice_queue + old_hi_size + (splice_queue_size % CHAR_BIT > 0), 0, new_hi_size - (splice_queue_size % CHAR_BIT > 0));
                                                      }

                                                      splice_queue[old_hi_size] |= trigraph << (splice_queue_size % CHAR_BIT);
                                                      splice_queue_size++;
                                                      trigraph = 0;
                                                      goto contin;
                                           default:
                                           failure:   putchar('/');
                                                      for (size_t x = 0; x < splice_queue_size; x++) {
                                                          puts((splice_queue[x / CHAR_BIT] & 1 << (x % CHAR_BIT)) ? "\?\?/" : "\\");
                                                      }
                                                      splice_queue_size = 0;
                                                      printf("%.*s", (int) qmark_count, "\?\?");
                                                      state = root;
                                                      break;
                                       }
                                       break;
        }

        putchar(c);
contin: qmark_count = 0;
    }
}