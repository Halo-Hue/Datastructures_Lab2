// ============================================================================
// list.c -- YOUR WORK for Lab 1.
//
// Implement every function below so it matches its contract in list.h. Each has
// a TODO describing what to do; the header has the ownership rule and Big-O.
//
// The placeholder bodies let the project compile and stay leak-free BEFORE you
// start, so the whole workflow (make, make valgrind, make format) works from
// minute one. As you implement each function, real behaviour appears.
//
// The `(void)x;` lines just tell the compiler "I know x is unused for now" so
// -Werror does not stop you. Delete each one once you use that variable.
// ============================================================================

#include "list.h"

#include <stdlib.h> // you will need malloc and free

TokenList list_create(void) {
    // GIVEN: an empty list is all-null with length 0. (This one is done for you
    // as an example of a designated initializer -- study it, then build the
    // rest below.)
    return (TokenList){.head = nullptr, .tail = nullptr, .length = 0};
}

void list_destroy(TokenList *list) {
    TokenNode *current = list->head;

    while (current != nullptr) {
        TokenNode *next = current->next;
        token_free(&current->token);
        free(current);
        current = next;
    }
    *list = list_create();
}

bool list_push_front(TokenList *list, Token token) {
    // TODO: allocate a node (malloc), store `token` in it, and link it at the
    // FRONT (new node's next = old head; head = new node). If the list was empty    // (tail ==
    // nullptr), the new node is also the tail. Then length++. On malloc failure:
    // token_free(&token); return false;  (do not leak!)
    TokenNode *new_token_node = malloc(sizeof(TokenNode));
    if (new_token_node == nullptr) {
        token_free(&token);
        return false;
    }
    new_token_node->token = token;
    new_token_node->next  = list->head;
    list->head            = new_token_node;

    if (list->tail == nullptr) {
        list->tail = new_token_node;
    }

    list->length++;
    return true;
}

bool list_push_back(TokenList *list, Token token) {
    // TODO: allocate a node holding `token` and append it at the BACK. If the
    // list is empty, it becomes both head and tail; otherwise link tail->next to
    // it and move tail. Then length++. Same malloc-failure rule as above.
    TokenNode *new_token_node = malloc(sizeof(TokenNode));
    if (new_token_node == nullptr) {
        token_free(&token);
        return false;
    }

    new_token_node->token = token;
    new_token_node->next  = nullptr;

    if (list->tail == nullptr) {
        list->head = new_token_node;
        list->tail = new_token_node;
    } else {
        list->tail->next = new_token_node;
        list->tail       = new_token_node;
    }
    list->length++;
    return true;
}

bool list_insert_after(TokenList *list, TokenNode *node, Token token) {
    // TODO: insert a new node holding `token` immediately after `node`. Careful
    // with the two links: new->next = node->next; node->next = new. If `node`
    // was the tail, update tail. Then length++. If `node` is nullptr, or malloc
    // fails, token_free(&token) and return false.
    if (node == nullptr) {
        token_free(&token);
        return false;
    }

    TokenNode *new_token_node = malloc(sizeof(TokenNode));
    if (new_token_node == nullptr) {
        token_free(&token);
        return false;
    }

    new_token_node->token = token;
    new_token_node->next  = node->next;
    node->next            = new_token_node;

    if (list->tail == node) {
        list->tail = new_token_node;
    }

    list->length++;
    return true;
}

TokenNode *list_find_first(const TokenList *list, TokenKind kind) {
    // TODO: walk from head; return the first node whose token.kind == kind, or
    // nullptr if none is found.
    TokenNode *current = list->head;

    while (current != nullptr) {

        if (current->token.kind == kind) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool list_remove_first(TokenList *list, TokenKind kind) {
    // TODO: find the first node with this kind while remembering the node BEFORE
    // it (`prev`). Unlink it (mind the head case where prev is nullptr, and the
    // tail case where you must move tail back to prev). Free its token then the
    // node. length--. Return true if removed, false if not found.

    TokenNode *current = list->head;
    TokenNode *prev    = nullptr;

    while (current != nullptr) {
        if (current->token.kind == kind) {
            if (prev == nullptr) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }

            if (current == list->tail) {
                list->tail = prev;
            }

            token_free(&current->token);
            free(current);
            list->length--;

            return true;
        }

        prev    = current;
        current = current->next;
    }
    return false;
}

size_t list_length(const TokenList *list) {

    return list->length;
}

void list_print(const TokenList *list, FILE *out) {
    // TODO: for each node, print one line to `out`. Suggested format (matches
    // the reference so grading scripts line up):
    //   fprintf(out, "  %-9s '%s' (%d:%d)\n",
    //           token_kind_name(cur->token.kind), cur->token.lexeme,
    //           cur->token.line, cur->token.col);

    TokenNode *current = list->head;

    while (current != nullptr) {
        fprintf(out, "  %-9s '%s' (%d:%d)\n", token_kind_name(current->token.kind),
                current->token.lexeme, current->token.line, current->token.col);

        current = current->next;
    }
}

void list_to_dot(const TokenList *list, FILE *out) {
    // TODO (do this LAST, after the list works): print Graphviz DOT to `out`.
    // A minimal version is fine to start:
    //   digraph tokens { rankdir=LR; node [shape=record];
    //     n0 [label="{KIND|lexeme}"]; n0 -> n1; ... last -> nnull; }
    // See list.h and the README for what the picture should look like.
    fprintf(out, "digraph tokens {\n");
    fprintf(out, "    rankdir=LR;\n");
    fprintf(out, "    node [shape=record];\n");

    size_t index       = 0;
    TokenNode *current = list->head;

    while (current != nullptr) {
        if (current->token.kind == TOK_ARROW) {
            fprintf(out, "    n%zu [label=\"{%s|-\\>}\"];\n", index,
                    token_kind_name(current->token.kind));
        } else {
            fprintf(out, "    n%zu [label=\"{%s|%s}\"];\n", index,
                    token_kind_name(current->token.kind), current->token.lexeme);
        }

        if (current->next != nullptr) {
            fprintf(out, "    n%zu -> n%zu;\n", index, index + 1);
        }
        index++;
        current = current->next;
    }
    fprintf(out, "}\n");
}

bool list_check_invariant(const TokenList *list) {
    // TODO: return true only if the list is well formed:
    //   - length == number of nodes reachable from head
    //   - tail is the last node, and is nullptr exactly when the list is empty
    // Returning true here is a placeholder; implement it so it can actually
    // catch a bookkeeping mistake for you.
    size_t count       = 0;
    TokenNode *current = list->head;

    while (current != nullptr) {
        count++;
        current = current->next;
    }
    if (count != list->length) {
        return false;
    }
    TokenNode *last = list->head;

    while (last != nullptr && last->next != nullptr) {
        last = last->next;
    }
    if (last != list->tail) {
        return false;
    }
    if (list->head == nullptr && list->tail != nullptr) {
        return false;
    }
    return true;
}
