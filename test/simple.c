/**
 * fcontext simple functionality test
 *
 *                Copyright Nick Vrvilo 2017.
 *            https://github.com/DaoWen/fcontext/
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE_1_0.txt or copy at
 *           http://www.boost.org/LICENSE_1_0.txt)
 */

#include "fcontext.h"

int ret_val;

void child_fn(fcontext_transfer_t arg) {
    fcontext_t parent = arg.prev_context;
    ret_val = 0;
    fcontext_swap(parent, NULL);
}

int main() {
    ret_val = 1;
    fcontext_state_t *self = fcontext_create_proxy();
    fcontext_state_t *child = fcontext_create(child_fn);
    fcontext_swap(child->context, NULL);
    fcontext_destroy(child);
    fcontext_destroy_proxy(self);
    return ret_val;
}
