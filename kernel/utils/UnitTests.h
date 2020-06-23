#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
#include "assert.h"
#include "bitmap.h"
#include "list.h"

#define ASSERT_TEST(expr, result) (printStatus("Testing: " #expr " == " #result, (expr == result)))

void UnitTest_CircularQueue();
void UnitTest_Bitmap();