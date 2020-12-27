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


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set contents
 * are iterated over to verify that iteration produces the correct sequence of
 * values.
 */
void test_iter_all_orders(TestContext &ctx, int n) {
    vector<int> add_order, del_order;
    for (int i = 0; i < n; i++)
        add_order.push_back(i);

    while (next_permutation(add_order.begin(), add_order.end())) {
        // Add all values to the tree-set, in the order specified.
        TreeSet s;
        add_values(ctx, s, add_order);

        // Check that all of the values are present.
        ctx.CHECK(s.size() == (int) add_order.size());
        for (int i = 0; i < n; i++)
            ctx.CHECK(s.contains(i));

        // Iterate over the values in the array, and make sure that each value
        // appears.
        TreeSet::iterator it = s.begin();
        for (int i = 0; i < n; i++) {
            ctx.CHECK(*it == i);
            it++;
        }
    }
}


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set is
 * compared to a "reference" tree-set containing the same values, to ensure that
 * equality and inequality work correctly.
 */
void test_equal_all_orders(TestContext &ctx, int n) {
    vector<int> add_order;
    for (int i = 0; i < n; i++)
        add_order.push_back(i);

    TreeSet orig;
    add_values(ctx, orig, add_order);

    TreeSet empty;

    while (next_permutation(add_order.begin(), add_order.end())) {
        // Add all values to the tree-set, in the order specified.
        TreeSet s;
        add_values(ctx, s, add_order);

        // Check that all of the values are present.
        ctx.CHECK(s.size() == (int) add_order.size());
        for (int i = 0; i < n; i++)
            ctx.CHECK(s.contains(i));

        // Check equality and inequality.

        ctx.CHECK(s == orig);
        ctx.CHECK(!(s != orig));

        ctx.CHECK(s != empty);
    }
}


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set is output
 * to an output-stream, so that the stream-output functionality can be verified.
 */
void test_ostream_all_orders(TestContext &ctx, int n) {
    ostringstream os;
    os << '[';
    bool first = true;

    vector<int> add_order;
    for (int i = 0; i < n; i++) {
        add_order.push_back(i);

        if (first)
            first = false;
        else
            os << ',';

        os << i;
    }
    os << ']';

    string expected = os.str();

    while (next_permutation(add_order.begin(), add_order.end())) {
        // Add all values to the tree-set, in the order specified.
        TreeSet s;
        add_values(ctx, s, add_order);

        // Check that all of the values are present.
        ctx.CHECK(s.size() == (int) add_order.size());
        for (int i = 0; i < n; i++)
            ctx.CHECK(s.contains(i));

        // Output the tree-set to the stream.
        os.str("");
        os << s;
        string actual = os.str();

        ctx.CHECK(actual == expected);
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


/*! Test the copy-constructor of the tree-set. */
void test_treeset_copy_ctor(TestContext &ctx) {
    ctx.DESC("Basic copy-constructor tests");

    TreeSet s1;
    s1.add(1);
    s1.add(2);
    s1.add(3);

    TreeSet s2{s1};

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 3);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));

    ctx.CHECK(s2.contains(1));
    ctx.CHECK(s2.contains(2));
    ctx.CHECK(s2.contains(3));

    s1.add(4);
    s2.del(2);

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 2);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK( s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK( s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    ctx.result();
}


/*! Test the copy-assignment operator of the tree-set. */
void test_treeset_copy_assign(TestContext &ctx) {
    ctx.DESC("Basic copy-assignment tests");

    TreeSet s1, s2, s3;
    s1.add(1);
    s1.add(2);
    s1.add(3);

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 0);
    ctx.CHECK(s3.size() == 0);

    // Assign non-empty to empty.
    s2 = s1;

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 3);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));

    ctx.CHECK(s2.contains(1));
    ctx.CHECK(s2.contains(2));
    ctx.CHECK(s2.contains(3));

    s1.add(4);
    s2.del(2);

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 2);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK( s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK( s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    // Assign empty to non-empty.
    s2 = s3;

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 0);
    ctx.CHECK(s3.size() == 0);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK(!s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK(!s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    // Self-assignment!
    (s1 = s1 = s1) = (s1 = s1 = s1);

    ctx.CHECK(s1.size() == 4);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

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


void test_iter_basic(TestContext &ctx) {
    {
        ctx.DESC("Iterator on empty set");
        TreeSet s;
        ctx.CHECK(s.begin() == s.end());
        ctx.result();
    }

    {
        ctx.DESC("Iterator on one-element set");
        TreeSet s;
        s.add(35);

        TreeSet::iterator it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 35);

        TreeSet::iterator old = it++;
        ctx.CHECK(it == s.end());
        ctx.CHECK(old != s.end());
        ctx.CHECK(*old == 35);

        ++old;
        ctx.CHECK(old == s.end());

        ctx.result();
    }

    {
        ctx.DESC("Iterator on two-element set (order 1)");
        TreeSet s;
        s.add(22);
        s.add(44);

        TreeSet::iterator it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 22);

        it++;
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 44);

        ++it;
        ctx.CHECK(it == s.end());

        ctx.result();
    }

    {
        ctx.DESC("Iterator on two-element set (order 2)");
        TreeSet s;
        s.add(44);
        s.add(22);

        TreeSet::iterator it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 22);

        it++;
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 44);

        ++it;
        ctx.CHECK(it == s.end());

        ctx.result();
    }
}


void test_iter_brute_force(TestContext &ctx) {
    ctx.DESC("Add/iterate over all sequences (3 values)");
    test_iter_all_orders(ctx, 3);
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (4 values)");
    test_iter_all_orders(ctx, 4);
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (5 values)");
    test_iter_all_orders(ctx, 5);
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (6 values)");
    test_iter_all_orders(ctx, 6);
    ctx.result();
}


void test_initializer_lists(TestContext &ctx) {
    {
        ctx.DESC("Initializer list (1 value)");
        TreeSet s = {5};
        ctx.CHECK(s.size() == 1);
        ctx.CHECK(s.contains(5));
        ctx.result();
    }

    {
        ctx.DESC("Initializer list (3 unique values)");
        TreeSet s = {5, 4, 9};
        ctx.CHECK(s.size() == 3);
        ctx.CHECK(s.contains(4));
        ctx.CHECK(s.contains(5));
        ctx.CHECK(s.contains(9));
        ctx.result();
    }

    {
        ctx.DESC("Initializer list (3 non-unique values)");
        TreeSet s = {5, 4, 5};
        ctx.CHECK(s.size() == 2);
        ctx.CHECK(s.contains(4));
        ctx.CHECK(s.contains(5));
        ctx.result();
    }
}


void test_basic_equality(TestContext &ctx) {
    TreeSet s1, s1b;
    TreeSet s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet s3{1, 2, 4};
    TreeSet s4{1, 2, 3, 4};

    ctx.DESC("Basic equality/inequality");

    ctx.CHECK(s1 == s1);
    ctx.CHECK(s1 == s1b);
    ctx.CHECK(!(s1 != s1));
    ctx.CHECK(!(s1 != s1b));

    ctx.CHECK(s2 == s2);
    ctx.CHECK(s2 == s2b);  // Different ordering
    ctx.CHECK(!(s2 != s2));
    ctx.CHECK(!(s2 != s2b));

    ctx.CHECK(s1 != s2);
    ctx.CHECK(s2 != s1);
    ctx.CHECK(s2 != s3);
    ctx.CHECK(s3 != s2);

    ctx.CHECK(!(s1 == s2));
    ctx.CHECK(!(s2 == s1));
    ctx.CHECK(!(s2 == s3));
    ctx.CHECK(!(s3 == s2));

    ctx.CHECK(s2 != s4);
    ctx.CHECK(s3 != s4);

    ctx.CHECK(!(s2 == s4));
    ctx.CHECK(!(s3 == s4));

    ctx.result();
}


void test_equal_brute_force(TestContext &ctx) {
    ctx.DESC("Equality/inequality over all sequences (3 values)");
    test_equal_all_orders(ctx, 3);
    ctx.result();

    ctx.DESC("Equality/inequality over all sequences (4 values)");
    test_equal_all_orders(ctx, 4);
    ctx.result();

    ctx.DESC("Equality/inequality over all sequences (5 values)");
    test_equal_all_orders(ctx, 5);
    ctx.result();

    ctx.DESC("Equality/inequality over all sequences (6 values)");
    test_equal_all_orders(ctx, 6);
    ctx.result();
}


void test_basic_ostream(TestContext &ctx) {
    TreeSet s1;
    TreeSet s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet s3{4, 1, 3, 2};

    ostringstream os;

    ctx.DESC("Basic stream-output");

    os.str("");
    os << s1;
    ctx.CHECK(os.str() == "[]");

    os.str("");
    os << s2;
    ctx.CHECK(os.str() == "[1,2,3]");

    os.str("");
    os << s2b;
    ctx.CHECK(os.str() == "[1,2,3]");

    os.str("");
    os << s3;
    ctx.CHECK(os.str() == "[1,2,3,4]");

    ctx.result();
}


void test_ostream_brute_force(TestContext &ctx) {
    ctx.DESC("Stream-output over all sequences (3 values)");
    test_ostream_all_orders(ctx, 3);
    ctx.result();

    ctx.DESC("Stream-output over all sequences (4 values)");
    test_ostream_all_orders(ctx, 4);
    ctx.result();

    ctx.DESC("Stream-output over all sequences (5 values)");
    test_ostream_all_orders(ctx, 5);
    ctx.result();

    ctx.DESC("Stream-output over all sequences (6 values)");
    test_ostream_all_orders(ctx, 6);
    ctx.result();
}


void test_set_ops(TestContext &ctx) {
    TreeSet s1;
    TreeSet s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet s3{1, 4, 2};
    TreeSet s4{4, 1, 3, 2};
    TreeSet s5{1, 2};
    TreeSet s6{3};
    TreeSet s7{4};

    TreeSet s;

    ctx.DESC("Basic set-union test");

    s = s1.plus(s2);
    ctx.CHECK(s == s2);

    s = s2.plus(s1);
    ctx.CHECK(s == s2);

    s = s2.plus(s2b);
    ctx.CHECK(s == s2);

    s = s2.plus(s3);
    ctx.CHECK(s == s4);

    s = s3.plus(s2);
    ctx.CHECK(s == s4);

    ctx.result();

    ctx.DESC("Basic set-intersect test");

    s = s1.intersect(s2);
    ctx.CHECK(s == s1);

    s = s2.intersect(s1);
    ctx.CHECK(s == s1);

    s = s2.intersect(s2b);
    ctx.CHECK(s == s2);

    s = s2.intersect(s2);
    ctx.CHECK(s == s2);

    s = s2.intersect(s3);
    ctx.CHECK(s == s5);

    s = s3.intersect(s4);
    ctx.CHECK(s == s3);

    ctx.result();

    ctx.DESC("Basic set-difference test");

    s = s1.minus(s2);
    ctx.CHECK(s == s1);

    s = s2.minus(s1);
    ctx.CHECK(s == s2);

    s = s2.minus(s2b);
    ctx.CHECK(s == s1);

    s = s2.minus(s2);
    ctx.CHECK(s == s1);

    s = s2.minus(s3);
    ctx.CHECK(s == s6);

    s = s3.minus(s2);
    ctx.CHECK(s == s7);

    s = s3.minus(s4);
    ctx.CHECK(s == s1);

    s = s4.minus(s3);
    ctx.CHECK(s == s6);

    ctx.result();
}


/*! This program is a simple test-suite for the TreeSet class. */
int main() {
  
    cout << "Testing the TreeSet class." << endl << endl;

    TestContext ctx(cout);

    test_treeset_default_ctor(ctx);
    test_basic_add_contains_size(ctx);
    test_basic_add_del_2(ctx);
    test_add_del_brute_force(ctx);

    test_treeset_copy_ctor(ctx);
    test_treeset_copy_assign(ctx);

    test_iter_basic(ctx);
    test_iter_brute_force(ctx);

    test_initializer_lists(ctx);

    test_basic_equality(ctx);
    test_equal_brute_force(ctx);

    test_basic_ostream(ctx);
    test_ostream_brute_force(ctx);

    test_set_ops(ctx);

    // Return 0 if everything passed, nonzero if something failed.
    return !ctx.ok();
}

