/*
 * Copyright 2014 Gregory Prisament
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Compile with:
 *
 *  gcc -I ../libcanopy/3rdparty/libred/include sddl_test.c -lcanopy -lcurl -lwebsockets
 */
#include "sddl.h"
#include "red_test.h"


int main(int argc, const char *argv[])
{
    RedTest suite = RedTest_Begin(argv[0], NULL, NULL);
    int result;

    /* Try parsing bad input files */
    {
        const char * BAD_SDDL =
        "hi\n"
        "{is this a valid file?\n"
        "}\n"
        "\"\"";

        SDDLParseResult pr;
        pr = sddl_parse(BAD_SDDL);
        RedTest_Verify(suite, "sddl_parse(BAD_SDDL) returns not ok", !sddl_parse_result_ok(pr));
        sddl_free_parse_result(pr);
    }

    /* Bad SDDL input #2 */
    {
        const char * BAD_SDDL2 =
        "{\n"
        "    \"key\" : \"value\n"     /* forgot to close quotes */
        "    \"key2\" : \"value2\"\n"
        "}\n";

        SDDLParseResult pr;
        pr = sddl_parse(BAD_SDDL2);
        RedTest_Verify(suite, "sddl_parse(BAD_SDDL2) returns not ok", !sddl_parse_result_ok(pr));
        sddl_free_parse_result(pr);
    }

    /* Bad SDDL input #3 */
    {
        const char * BAD_SDDL3 =
        "{\n"
        "    \"key\" : \"value\n"     /* forgot to close quotes */
        "}\n";

        SDDLParseResult pr;
        pr = sddl_parse(BAD_SDDL3);
        RedTest_Verify(suite, "sddl_parse(BAD_SDDL3) returns not ok", !sddl_parse_result_ok(pr));
        sddl_free_parse_result(pr);
    }

    /* Minimal SDDL 
     *
     * Test that {} is considered a valid SDDL file.
     *
     */
    {
        const char * MINIMAL_SDDL = "{}";

        SDDLParseResult pr;
        SDDLDocument doc;
        pr = sddl_parse(MINIMAL_SDDL);
        RedTest_Verify(suite, "sddl_parse(MINIMAL_SDDL) returns ok", sddl_parse_result_ok(pr));
        RedTest_Verify(suite, "sddl_parse(MINIMAL_SDDL) no errors", sddl_parse_result_num_errors(pr) == 0);
        RedTest_Verify(suite, "sddl_parse(MINIMAL_SDDL) no warnings", sddl_parse_result_num_warnings(pr) == 0);
        doc = sddl_parse_result_ref_document(pr);
        sddl_free_parse_result(pr);

        RedTest_Verify(suite, "Minimal SDDL - doc generated", doc != NULL);
        RedTest_Verify(suite,
            "Minimal SDDL - doc description default matches spec",
            !strcmp(sddl_document_description(doc), ""));
        RedTest_Verify(suite,
            "Minimal SDDL - no authors",
            sddl_document_num_authors(doc) == 0);
        RedTest_Verify(suite,
            "Minimal SDDL - no properties",
            sddl_document_num_properties(doc) == 0);
    }

    result = RedTest_End(suite);
    if (!result)
    {
        printf("All tests passed.\n");
    }
    return result;
}
