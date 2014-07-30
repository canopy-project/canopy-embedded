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

    /* SDDL Empty Classes
     *
     * Test example SDDL from specification document:
     * {
     *     "class miele.smart_oven_3000" : { },
     *     "class dyson.fans.dyson_cool.am08" : { },
     * }
     *
     */
    {
        const char * EMPTY_CLASSES_SDDL = 
            "{\n"
            "    \"class miele.smart_oven_3000\" : { },\n"
            "    \"class dyson.fans.dyson_cool.am08\" : { },\n"
            "}\n";

        SDDLParseResult pr;
        SDDLDocument doc;
        SDDLProperty propMiele, propDyson;
        pr = sddl_parse(EMPTY_CLASSES_SDDL);
        RedTest_Verify(suite, "sddl_parse(EMPTY_CLASSES_SDDL) returns ok", sddl_parse_result_ok(pr));
        RedTest_Verify(suite, "sddl_parse(EMPTY_CLASSES_SDDL) no errors", sddl_parse_result_num_errors(pr) == 0);
        RedTest_Verify(suite, "sddl_parse(EMPTY_CLASSES_SDDL) no warnings", sddl_parse_result_num_warnings(pr) == 0);
        doc = sddl_parse_result_ref_document(pr);
        sddl_free_parse_result(pr);

        RedTest_Verify(suite, "Empty Classes SDDL - doc generated", doc != NULL);
        RedTest_Verify(suite,
            "Empty Classes SDDL - doc description default matches spec",
            !strcmp(sddl_document_description(doc), ""));
        RedTest_Verify(suite,
            "Empty Classes SDDL - no authors",
            sddl_document_num_authors(doc) == 0);

        RedTest_Verify(suite,
            "Empty Classes SDDL - 2 properties",
            sddl_document_num_properties(doc) == 2);

        propMiele = sddl_document_property(doc, 0);
        RedTest_Verify(suite,
            "Empty Classes SDDL - sddl_document_property 0 non-null",
            propMiele != NULL);
        RedTest_Verify(suite,
            "Empty Classes SDDL - property 0 is class",
            sddl_is_class(propMiele));
        if (!strcmp(sddl_class_name(SDDL_CLASS(propMiele)), "miele.smart_oven_3000"))
        {
            propDyson = sddl_document_property(doc, 1);
        }
        else
        {
            propDyson = propMiele;
            propMiele = sddl_document_property(doc, 1);
        }

        RedTest_Verify(suite,
            "Empty Classes SDDL - No authors",
            sddl_class_num_authors(SDDL_CLASS(propMiele)) == 0);
        RedTest_Verify(suite,
            "Empty Classes SDDL - No authors (2)",
            sddl_class_num_authors(SDDL_CLASS(propDyson)) == 0);
        RedTest_Verify(suite,
            "Empty Classes SDDL - No description",
            !strcmp(sddl_class_description(SDDL_CLASS(propMiele)), ""));
        RedTest_Verify(suite,
            "Empty Classes SDDL - No description (2)",
            !strcmp(sddl_class_description(SDDL_CLASS(propDyson)), ""));
        RedTest_Verify(suite,
            "Empty Classes SDDL - No properties",
            sddl_class_num_properties(SDDL_CLASS(propMiele)) == 0);
        RedTest_Verify(suite,
            "Empty Classes SDDL - No properties (2)",
            sddl_class_num_properties(SDDL_CLASS(propMiele)) == 0);
    }

    result = RedTest_End(suite);
    if (!result)
    {
        printf("All tests passed.\n");
    }
    return result;
}
