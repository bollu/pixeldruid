#include <isl/set.h>
#include <isl/map.h>
#include <iostream>

using namespace std;

isl_stat print_point(isl_point *pt, void *data) {
    isl_map *m = (isl_map *)data;
    isl_basic_set *ptset = isl_basic_set_from_point(isl_point_copy(pt));
    isl_set *app = isl_set_apply(isl_set_from_basic_set(ptset), isl_map_copy(m));
    std::cout <<"\n" << isl_point_to_str(pt) << "->" << isl_set_to_str(app);
    return isl_stat_ok;
}


/*
int main() {
    isl_ctx *ctx = isl_ctx_alloc();
    //
    // app Cons nil xs = xs
    // app (Cons a b) xs = let b' = b ++ xs in Cons a b'
    // ----
    //
    // in:(nil, cons a b), | in:xs,   | out:nil, (cons a', cons b')
    // APP[ 0         1 2   | 3     ] |  OUT[4          5         6]
    //
    // ------------------------------
    // (4) out:nil -> (0) in:nil 
    // (5) out:cons a' -> (1) in:cons a
    // (6) out:cons b' -> (2) in:cons b
    // (6) out:cons b' -> (3) in:cons xs
    //
    // (5) out:cons a' -> (1) in:cons a
    // (6) out:cons b' -> (3) in:cons b
    //
    isl_map *m = isl_map_read_from_str(ctx, "{ \
            a[x] -> a[x-4+0]: x mod 7 = 4; \
            a[x] -> a[x-5+1]: x mod 7 = 5; \
            a[x] -> a[x-6+2]: x mod 7 = 6; \
            a[x] -> a[x-6+3]: x mod 7 = 6; \
            a[x] -> a[x-5-7+1]: x mod 7 = 5; \
            a[x] -> a[x-6-7+2]: x mod 7 = 6; \
        }");
    std::cout << "built map\n";
    isl_map *pow_ = isl_map_power(isl_map_copy(m), NULL);
    isl_map *trans_ = isl_map_transitive_closure(m, NULL);


    isl_printer *p = isl_printer_to_str(ctx);
    isl_printer_print_map(p, pow_);
    std::cout <<"\nPOWER: "<<  isl_printer_get_str(p);

    isl_printer_free(p);
    p = isl_printer_to_str(ctx);
    isl_printer_print_map(p, trans_);
    std::cout <<"\nTRANS: "<<isl_printer_get_str(p);

    isl_set *domain = NULL;


    // ==================================
    // TRANS: { a[x] -> a[o0] : (x) mod 7 = 6 and -4 + x <= o0 <= -3 + x; 
    //          a[x] -> a[-4 + x] : -5 + x <= 7*floor((x)/7) <= -4 + x; 
    //          a[x] -> a[-11 + x] : 7*floor((x)/7) <= -5 + x }
    //
    //
    // CONSTRAINT 1
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]: (x) mod 7 = 6 and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));

    // CONSTRAINT 2
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]:  -5 + x <= 7*floor((x)/7) <= -4 + x and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));
    
    // CONSTRAINT 3
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]:  7*floor((x)/7) <= -5 + x and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));
    
    //app = isl_set_apply(domain, trans_);
    //std::cout<<"\nAPP: " << isl_set_to_str(app);
    //isl_set_foreach_point(app, print_point, NULL);

    return 0;
}
*/


int main() {
    isl_ctx *ctx = isl_ctx_alloc();
    //-- app Nil ys = ys
    //app Nil Nil = Nil
    //app Nil (Cons y ys) = Cons y ys
    //
    //-- app (Cons x xs) ys = 
    //--     let xs' = app xs  ys in 
    //--     Cons x xs'
    //
    //app (Cons x xs) Nil = 
    //    let xs' = app xs Nil in 
    //    Cons x xs'
    //
    //app (Cons x xs) (Cons y ys) = 
    //    let xs' = app xs (Cons y ys) in 
    //    Cons x xs'
    // in1:nil|(cons x xs) in2:nil|(cons y ys) out:nil|(cons z zs)
    //     0         1  2      0         1  2       0        1   2
    //
    // relation between outputs and inputs, through recursion hierarchy
    // head (tail* ())
    // TODO: assume we know that lists are linear, and that trees can be 
    // accessed as (2k, 2k+1).
    // ----------------------
    // Does this allow us to encode data?
    
    isl_map *m = isl_map_read_from_str(ctx, "{" 
            // head (app (cons x xs (nil)) = head(Cons x xs') = x
            // force head: 0
            // force tail: 1
            "R[t1, t2, forced=0, out] -> R[t1, t2, forced, t1] : t1 % 3 = 1 and t2 % 3 = 0"
            // tail (app (cons x xs) (nil)) = tail(Cons x xs') = xs'
            // xs' = app xs Nil
            // force head: 0
            // force tail: 1
            "R[t1, t2, forced=1, out] -> R[t1, t2, forced, t1] : t1 % 3 = 1 and t2 % 3 = 0"
        "}");
    std::cout << "built map: " << isl_map_to_str(m) << "\n\n";
    isl_map *pow_ = isl_map_power(isl_map_copy(m), NULL);
    isl_map *trans_ = isl_map_transitive_closure(m, NULL);


    isl_printer *p = isl_printer_to_str(ctx);
    isl_printer_print_map(p, pow_);
    std::cout <<"\nPOWER: "<<  isl_printer_get_str(p);

    isl_printer_free(p);
    p = isl_printer_to_str(ctx);
    isl_printer_print_map(p, trans_);
    std::cout <<"\nTRANS: "<<isl_printer_get_str(p);

    isl_set *domain = NULL;


    // CONSTRAINT 1
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]: (x) mod 7 = 6 and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));

    // CONSTRAINT 2
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]:  -5 + x <= 7*floor((x)/7) <= -4 + x and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));
    
    // CONSTRAINT 3
    std::cout << "\n===========================\n";
    domain = isl_set_read_from_str(ctx, "{a[x]:  7*floor((x)/7) <= -5 + x and -20 <= x <= 20}");
    std::cout << "\nDOMAIN: " << isl_set_to_str(domain);
    isl_set_foreach_point(domain, print_point, isl_map_copy(trans_));

    return 0;
}
