This file contains the C coding style conventions for the Canopy project.

WHITESPACE CONVENTIONS
----------------------

Use soft tabs of 4 spaces:

    if (true)
        do_something();  // <--- Indented 4 spaces

Curly-braces typically go on their own lines:

    int main(void)
    {
        if (true)
        {
            do_something();
        }
        else
        {
            do_something_else();
        }
    }

Switch statements are written like this:

    switch (value)
    {
        case 0:
        {
            do_something();
            break;
        }
        case 1:
        {
            do_something_else();
            break;
        }
        default:
        {
            do_default_thing();
            break;
        }
    }

Do-while loops are written like this:

    do
    {

    } while (1)

Stay below 80 columns when possible.

Break up long lines by continuing with a 2-tab indent.

    if (some_really_long_condition && another_really_long_condition &&
            a_third_really_long_condition && final_really_long_condition)
    {
        return;
    }

Break up function calls with long parameter lists by putting one parameter on
each line (each indented by 8 spaces):

    really_long_function_call_with_lots_of_parameters(
            param1, 
            param2,
            param3, 
            param4, 
            param5);


NAMING CONVENTIONS
----------------------

    Function names:

        int function_names_are_lowercase_with_underscores();

    Only externally-exposed canopy library functions begin with "canopy_"

        canopy_service(NULL);

    Internal non-static routines begin with "st_" (for SimpleThings):

        st_websocket_write(...);

    Internal static routines begin with "_":

        _flush_logs();

    Variables and function parameters are camelback starting with lowercase:

        int myLocalVariable;

        int foo(void *firstParam, int secondParam);

    Structure members are lowercase with underscores:

        struct MyStruct
        {
            int num_items;
            void *items;
            char *who_let_the_dogs_out;
        };
