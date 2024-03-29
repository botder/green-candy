/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Tests.hpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef MTA_DEBUG

// Fwd decl
void    SString_Tests                   ( void );
void    SharedUtil_File_Tests           ( void );
void    SharedUtil_ClassIdent_Tests     ( void );
void 	SharedUtil_WildcardMatch_Tests  ( void );
void    SharedUtil_Collection_Tests     ( void );

///////////////////////////////////////////////////////////////
//
// Run tests entry point
//
//
//
///////////////////////////////////////////////////////////////
void SharedUtil_Tests ( void )
{
    SString_Tests ();
    SharedUtil_File_Tests ();
    SharedUtil_ClassIdent_Tests ();
    SharedUtil_WildcardMatch_Tests ();
    SharedUtil_Collection_Tests ();
}


///////////////////////////////////////////////////////////////
//
// Helper macros
//
//
//
///////////////////////////////////////////////////////////////
#define TEST_FUNCTION \
        struct testStruct { \
            void Test ( int testIndex ) \
            { \
                testStruct dataCopy = *this;    // Info when debugging

#define TEST_VARS \
            }

#define TEST_DATA \
        } testData[] = {


#define TEST_END \
            }; \
        for ( uint i = 0 ; i < NUMELMS( testData ) ; i++ ) \
            testData[i].Test ( i );


///////////////////////////////////////////////////////////////
//
// SString_Tests
//
// Tests for functions from SString.hpp
//
///////////////////////////////////////////////////////////////
void SString_Tests ( void )
{
    // vFormat
    {
        TEST_FUNCTION
            assert ( SString ( a, b, c ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            int c;
            const char* result;
        TEST_DATA
            "XX%sAA%dBB", "la",     0,      "XXlaAA0BB",
            "XX%sAA%dBB", "laQWE",  2000,   "XXlaQWEAA2000BB",
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
            SString strLeft, strRight;
            assert ( source.Split ( delim, &strLeft, &strRight, index ) == result );
            assert ( strLeft == resultLeft );
            assert ( strRight == resultRight );
        TEST_VARS
            SString source;
            const char* delim;
            int index;
            bool result;
            const char* resultLeft;
            const char* resultRight;
        TEST_DATA
            "a.b.c.d.e",  ".",    1,    true,   "a",        "b.c.d.e",
            "a.b.c.d.e",  ".",    2,    true,   "a.b",      "c.d.e",
            "a.b.c.d.e",  ".",    -2,   true,   "a.b.c",    "d.e",
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
            std::vector < SString > parts;
            source.Split ( delim, parts, maxAmmount, minAmmount );
            assert ( parts.front () == resultFirst );
            assert ( parts.back () == resultLast );
            assert ( parts.size () == resultCount );
        TEST_VARS
            SString source;
            const char* delim;
            uint maxAmmount;
            uint minAmmount;
            const char* resultFirst;
            const char* resultLast;
            uint resultCount;
        TEST_DATA
            "hello",            ".",    0,  0,  "hello",    "hello",    1,
            "a.#b.#c.#d.#e",    ".#",   0,  0,  "a",        "e",        5,
        TEST_END
    }

    // SplitLeft
    // SplitRight

    // Replace
    {
        TEST_FUNCTION
            assert ( source.Replace ( needle, newneedle ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            const char* result;
        TEST_DATA
            "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blah", "",     "1234",
            "blahblah1234", "blah", "",     "1234",
            "1234blahblah", "blah", "",     "1234",
            "1234blAhblaH", "BLah", "",     "1234blAhblaH",
            "blah1234blah", "LA",   "QwE",  "blah1234blah",
            "blah1234blah", "blAh", "",     "blah1234blah",
            "blah////blah", "//",   "/",    "blah//blah",
        TEST_END
    }


    // Replace with option
    {
        TEST_FUNCTION
            assert ( source.Replace ( needle, newneedle, bSearchJustReplaced ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            bool bSearchJustReplaced;
            const char* result;
        TEST_DATA
            "blah////blah", "//",   "/",  false,    "blah//blah",
            "blah////blah", "//",   "/",  true,     "blah/blah",
        TEST_END
    }


    // ReplaceI
    {
        TEST_FUNCTION
            assert ( source.ReplaceI ( needle, newneedle ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            const char* result;
        TEST_DATA
            "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blah", "",     "1234",
            "blahblah1234", "blAh", "",     "1234",
            "1234blAhblaH", "BLah", "",     "1234",
            "blah1234blah", "LA",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blAh", "",     "1234",
        TEST_END
    }

    // TrimStart
    // TrimEnd
    // ToLower
    // ToUpper
    // ConformLineEndings
    // Contains
    // ContainsI
    // SubStr
    // Left
    // Right
    // EndsWith
    // EndsWithI
    // BeginsWith
    // BeginsWithI

    // Join
    {
        TEST_FUNCTION
            std::vector < SString > parts;
            source.Split ( ",", parts );
            assert ( SString::Join ( delim, parts, first, count ) == result );
        TEST_VARS
            SString source;
            SString delim;
            int first;
            int count;
            SString result;
        TEST_DATA
            "",             "/",    0, 999, "",
            "A",            "/",    0, 999, "A",
            "A,B",          "/",    0, 999, "A/B",
            "A,B,C,D,E",    "/",    0, 999, "A/B/C/D/E",
            "A,B,C,D,E",    "/",   -5, 7,   "A/B",
            "",             "/",    0, 0,   "",
            "A",            "/",    0, 0,   "",
            "A,B",          "/",    0, 0,   "",
            "A,B,C,D,E",    "/",    0, 0,   "",
            "",             "/",    0, 1,   "",
            "A",            "/",    0, 1,   "A",
            "A,B",          "/",    0, 1,   "A",
            "A,B,C,D,E",    "/",    0, 1,   "A",
            "",             "/",    0, 2,   "",
            "A",            "/",    0, 2,   "A",
            "A,B",          "/",    0, 2,   "A/B",
            "A,B,C,D,E",    "/",    0, 2,   "A/B",
            "",             "/",    1, 2,   "",
            "A",            "/",    1, 2,   "",
            "A,B",          "/",    1, 2,   "B",
            "A,B,C,D,E",    "/",    1, 2,   "B/C",
            "A,B,C,D,E",    "/",    1, 4,   "B/C/D/E",
            "A,B,C,D,E",    "/",    1, 5,   "B/C/D/E",
        TEST_END
    }


}


///////////////////////////////////////////////////////////////
//
// SharedUtil_File_Tests
//
// Tests for functions from SharedUtil.File.hpp
//
///////////////////////////////////////////////////////////////
void SharedUtil_File_Tests ( void )
{
    return;
}


///////////////////////////////////////////////////////////////
//
// SharedUtil_ClassIdent_Tests
//
// Tests for macros in SharedUtil.ClassIdent.h
//
///////////////////////////////////////////////////////////////
void SharedUtil_ClassIdent_Tests ( void )
{
    // Setup
    enum eTestClassTypes
    {
        CLASS_SPlant,
        CLASS_STree,
        CLASS_SFlower,
        CLASS_SBlueBell,
        CLASS_SDaffodil,
    };

    struct SPlant
    {
        DECLARE_BASE_CLASS( SPlant )
        SPlant ( void ) : ClassInit ( this ) {}
    };

    struct STree : SPlant
    {
        DECLARE_CLASS( STree, SPlant )
        STree ( void ) : ClassInit ( this ) {}
    };

    struct SFlower : SPlant
    {
        DECLARE_CLASS( SFlower, SPlant )
        SFlower ( void ) : ClassInit ( this ) {}
    };

    struct SBlueBell : SFlower
    {
        DECLARE_CLASS( SBlueBell, SFlower )
        SBlueBell ( void ) : ClassInit ( this ) {}
    };

    struct SDaffodil : SFlower
    {
        DECLARE_CLASS( SDaffodil, SFlower )
        SDaffodil ( void ) : ClassInit ( this ) {}
    };

    // Create test data
    SPlant* pPlant = new SPlant ();
    STree* pTree = new STree ();
    SFlower* pFlower = new SFlower ();
    SBlueBell* pBlueBell = new SBlueBell ();
    SDaffodil* pDaffodil = new SDaffodil ();

    // Tests
    assert ( DynamicCast < SPlant > ( pPlant ) );
    assert ( DynamicCast < SPlant > ( pTree ) );
    assert ( DynamicCast < SPlant > ( pFlower ) );
    assert ( DynamicCast < SPlant > ( pBlueBell ) );
    assert ( DynamicCast < SPlant > ( pDaffodil ) );

    assert ( !DynamicCast < STree > ( pPlant ) );
    assert ( DynamicCast < STree > ( pTree ) );

    assert ( !DynamicCast < SFlower > ( pPlant ) );
    assert ( DynamicCast < SFlower > ( pFlower ) );
    assert ( DynamicCast < SFlower > ( pBlueBell ) );
    assert ( DynamicCast < SFlower > ( pDaffodil ) );

    assert ( !DynamicCast < SBlueBell > ( pPlant ) );
    assert ( !DynamicCast < SBlueBell > ( pFlower ) );
    assert ( DynamicCast < SBlueBell > ( pBlueBell ) );

    assert ( !DynamicCast < SDaffodil > ( pPlant ) );
    assert ( !DynamicCast < SDaffodil > ( pFlower ) );
    assert ( DynamicCast < SDaffodil > ( pDaffodil ) );

    // Create test data
    SPlant* pPlantTree = pTree;
    SPlant* pPlantFlower = pFlower;
    SPlant* pPlantBlueBell = pBlueBell;
    SPlant* pPlantDaffodil = pDaffodil;
    SFlower* pFlowerBlueBell = pBlueBell;
    SFlower* pFlowerDaffodil = pDaffodil;

    // Tests
    assert ( DynamicCast < SPlant > ( pPlantTree ) );
    assert ( DynamicCast < SPlant > ( pPlantFlower ) );
    assert ( DynamicCast < SPlant > ( pPlantBlueBell ) );
    assert ( DynamicCast < SPlant > ( pPlantDaffodil ) );
    assert ( DynamicCast < SPlant > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SPlant > ( pFlowerDaffodil ) );

    assert ( DynamicCast < STree > ( pPlantTree ) );
    assert ( !DynamicCast < STree > ( pPlantFlower ) );
    assert ( !DynamicCast < STree > ( pPlantBlueBell ) );
    assert ( !DynamicCast < STree > ( pPlantDaffodil ) );

    assert ( !DynamicCast < SFlower > ( pPlantTree ) );
    assert ( DynamicCast < SFlower > ( pPlantFlower ) );
    assert ( DynamicCast < SFlower > ( pPlantBlueBell ) );
    assert ( DynamicCast < SFlower > ( pPlantDaffodil ) );
    assert ( DynamicCast < SFlower > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SFlower > ( pFlowerDaffodil ) );

    assert ( !DynamicCast < SBlueBell > ( pPlantTree ) );
    assert ( !DynamicCast < SBlueBell > ( pPlantFlower ) );
    assert ( DynamicCast < SBlueBell > ( pPlantBlueBell ) );
    assert ( !DynamicCast < SBlueBell > ( pPlantDaffodil ) );
    assert ( DynamicCast < SBlueBell > ( pFlowerBlueBell ) );
    assert ( !DynamicCast < SBlueBell > ( pFlowerDaffodil ) );

    assert ( !DynamicCast < SDaffodil > ( pPlantTree ) );
    assert ( !DynamicCast < SDaffodil > ( pPlantFlower ) );
    assert ( !DynamicCast < SDaffodil > ( pPlantBlueBell ) );
    assert ( DynamicCast < SDaffodil > ( pPlantDaffodil ) );
    assert ( !DynamicCast < SDaffodil > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SDaffodil > ( pFlowerDaffodil ) );

    // Check ClassBits type is at least 64 bits
    assert ( sizeof ( ClassBits ) * 8 >= 64 );

    // Cleanup
    delete pPlant;
    delete pTree;
    delete pFlower;
    delete pBlueBell;
    delete pDaffodil;
}


///////////////////////////////////////////////////////////////
//
// SharedUtil_WildcardMatch_Tests
//
// Tests for WildcardMatch
//
///////////////////////////////////////////////////////////////
void SharedUtil_WildcardMatch_Tests ( void )
{
    // WildcardMatch
    {
        TEST_FUNCTION
            assert ( WildcardMatch ( a, b ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            bool result;
        TEST_DATA
            "*bd*",         "abcbde",               true,
            "*bd?f*",       "abcbdef_bdgh",         true,
            "*bd?h*",       "abcbdef_bdgh",         true,
            "*bd?g*",       "abcbdef_bdgh",         false,
            "scr*w?d",      "screeeewywxd",         true,
            "A*B",          "A_B_B",                true,
            "",             "",                     true,
            "*",            "",                     true,
            "*",            "A",                    true,
            "",             "A",                    false,
            "A*",           "",                     false,
            "A*",           "AAB",                  true,
            "A*",           "BAA",                  false,
            "A*",           "A",                    true,
            "A*B",          "",                     false,
            "A*B",          "AAB",                  true,
            "A*B",          "AB",                   true,
            "A*B",          "AABA",                 false,
            "A*B",          "ABAB",                 true,
            "A*B",          "ABBBB",                true,
            "A*B*C",        "",                     false,
            "A*B*C",        "ABC",                  true,
            "A*B*C",        "ABCC",                 true,
            "A*B*C",        "ABBBC",                true,
            "A*B*C",        "ABBBBCCCC",            true,
            "A*B*C",        "ABCBBBCBCCCBCBCCCC",   true,
            "A*B*",         "AB",                   true,
            "A*B*",         "AABA",                 true,
            "A*B*",         "ABAB",                 true,
            "A*B*",         "ABBBB",                true,
            "A*B*C*",       "",                     false,
            "A*B*C*",       "ABC",                  true,
            "A*B*C*",       "ABCC",                 true,
            "A*B*C*",       "ABBBC",                true,
            "A*B*C*",       "ABBBBCCCC",            true,
            "A*B*C*",       "ABCBBBCBCCCBCBCCCC",   true,
            "A?",           "AAB",                  false,
            "A?B",          "AAB",                  true,
            "A?*",          "A",                    false,
            "A?*",          "ABBCC",                true,
            "A?*",          "BAA",                  false,
        TEST_END
    }
}


///////////////////////////////////////////////////////////////
//
// SharedUtil_Collection_Tests
//
// Test behaviour of iterators and such
//
///////////////////////////////////////////////////////////////
void SharedUtil_Collection_Tests ( void )
{
    // std::map
    {
        std::map < uint, SString > testMap;
        MapSet ( testMap, 10, "10" );
        MapSet ( testMap, 20, "20" );
        MapSet ( testMap, 30, "30" );
        MapSet ( testMap, 40, "40" );

        std::map < uint, SString >::iterator iter = testMap.begin ();
        assert ( iter->first == 10 );
        iter++;
        assert ( iter->first == 20 );
        iter++;
        testMap.erase ( iter++ );
        assert ( iter->first == 40 );
        iter++;
        assert ( iter == testMap.end () );
    }

    // std::set
    {
        std::set < uint > testMap;
        MapInsert ( testMap, 10 );
        MapInsert ( testMap, 20 );
        MapInsert ( testMap, 30 );
        MapInsert ( testMap, 40 );

        std::set < uint >::iterator iter = testMap.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        testMap.erase ( iter++ );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testMap.end () );
    }

    // std::list
    {
        std::list < uint > testList;
        testList.push_back ( 10 );
        testList.push_back ( 20 );
        testList.push_back ( 30 );
        testList.push_back ( 40 );

        std::list < uint >::iterator iter = testList.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        iter = testList.erase ( iter );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testList.end () );
    }

    // std::vector
    {
        std::vector < uint > testList;
        testList.push_back ( 10 );
        testList.push_back ( 20 );
        testList.push_back ( 30 );
        testList.push_back ( 40 );

        std::vector < uint >::iterator iter = testList.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        iter = testList.erase ( iter );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testList.end () );
    }

}

#endif  // MTA_DEBUG
