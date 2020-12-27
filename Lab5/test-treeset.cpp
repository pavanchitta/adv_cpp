#include "testbase.h"
#include "treeset.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;


/*===========================================================================
 * HELPER FUNCTIONS
 *
 * These are used by various tests.
 */


/*!
 * Adds the specified collection of values to the tree-set, in order.  Every
 * add operation is expected to succeed.
 */
void add_values(TestContext &ctx, TreeSet &s, vector<int> values) {
    for (int n : values) {
        ctx.CHECK(s.add(n));
    }
}


/*!
 * Deletes the specified collection of values from the tree-set, in order.
 * Every delete operation is expected to succeed.
 */
void del_values(TestContext &ctx, TreeSet &s, vector<int> values) {
    for (int n : values) {
        ctx.CHECK(s.del(n));
    }    
}


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the values are also
 * deleted from the tree-set in all possible orderings.  Thus, this function has
 * a time complexity of approximately O(N^2 * log N).
 */
void test_add_del_all_orders(TestContext &ctx, int n) {
    vector<int> add_order, del_order;
    for (int i = 0; i < n; i++) {
        add_order.push_back(i);
        del_order.push_back(i);
    }

    while (next_permutation(add_order.begin(), add_order.end())) {
        while (next_permutation(del_order.begin(), del_order.end())) {
            // Add all values to the tree-set, in the order specified.
            TreeSet s;
            add_values(ctx, s, add_order);

            // Check that all of the values are present.
            ctx.CHECK(s.size() == (int) add_order.size());
            for (int i = 0; i < n; i++)
                ctx.CHECK(s.contains(i));

            // Delete all values from the tree-set in the order specified.
            del_values(ctx, s, del_order);

            // Check that none of the values are present.
            ctx.CHECK(s.size() == 0);
            for (int i = 0; i < n; i++)
                ctx.CHECK(!s.contains(i));
        }
    }
}


/*===========================================================================
 * TEST FUNCTIONS
 *
 * These are called by the main() function at the end of this file.
 */


/*! Test the default TreeSet constructor. */
void test_treeset_default_ctor(TestContext &ctx) {
    ctx.DESC("TreeSet default constructor");

    TreeSet s;
    ctx.CHECK(s.size() == 0);
    ctx.result();
}


/*! Test the basic add / contains / size functionality, with no deletion. */
void test_basic_add_contains_size(TestContext &ctx) {
    ctx.DESC("Basic add/contains/size tests");

    TreeSet s;

    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.add(123));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 1);

    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(!s.contains(78));
    ctx.CHECK(s.add(78));
    ctx.CHECK(s.contains(78));    
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 3);

    ctx.result();
}


/*! Test the basic add/del functionality, with only two values. */
void test_basic_add_del_2(TestContext &ctx) {
    ctx.DESC("Basic add/delete tests (2 values)");

    TreeSet s;

    // Add / delete a single value

    ctx.CHECK(!s.del(123));            // Not in the set yet

    ctx.CHECK(s.size() == 0);

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 1);

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.size() == 0);

    ctx.CHECK(!s.del(123));            // Should return false the second time

    // Add / delete multiple values (order 1)

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(!s.del(78));             // Not in the set

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 1);
    ctx.CHECK(!s.del(123));            // Should return false the second time

    ctx.CHECK(s.del(456));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 0);

    // Add / delete multiple values (order 2)

    ctx.CHECK(!s.del(123));            // Not in the set yet
    ctx.CHECK(!s.del(456));            // Not in the set yet

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(s.del(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 1);
    ctx.CHECK(!s.del(456));            // Should return false the second time

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 0);

    ctx.result();
}


void test_add_del_brute_force(TestContext &ctx) {
    ctx.DESC("Add/delete all sequences (3 values)");
    test_add_del_all_orders(ctx, 3);
    ctx.result();

    ctx.DESC("Add/delete all sequences (4 values)");
    test_add_del_all_orders(ctx, 4);
    ctx.result();

    ctx.DESC("Add/delete all sequences (5 values)");
    test_add_del_all_orders(ctx, 5);
    ctx.result();

    ctx.DESC("Add/delete all sequences (6 values)");
    test_add_del_all_orders(ctx, 6);
    ctx.result();

    // IF YOU ARE A GLUTTON FOR PUNISHMENT, UNCOMMENT THIS:
    // ctx.DESC("Add/delete all sequences (7 values)");
    // test_add_del_all_orders(ctx, 7);
    // ctx.result();
}


/*! This program is a simple test-suite for the TreeSet class. */
int main() {
  
    cout << "Testing the TreeSet class." << endl << endl;

    TestContext ctx(cout);

    test_treeset_default_ctor(ctx);
    test_basic_add_contains_size(ctx);
    test_basic_add_del_2(ctx);
    test_add_del_brute_force(ctx);

    // Return 0 if everything passed, nonzero if something failed.
    return !ctx.ok();
}

