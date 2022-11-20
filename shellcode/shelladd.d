import std.stdio;
//import std.array;
//import std.experimental.allocator.mmap_allocator;
import core.sys.posix.sys.mman;
import core.stdc.string: memcpy;
//import std.algorithm.mutation;

int main() {

    char[] code = ['\x55', '\x48', '\x89', '\xe5', '\x89', '\x7d', '\xfc', '\x89', '\x75', '\xf8', '\x8b', '\x55', '\xfc', '\x8b', '\x45', '\xf8', '\x01', '\xd0', '\x5d', '\xc3'];

    int* adder = cast(int*)mmap(null, code.length, PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

    //writeln(code.length);
    //writeln(adder.sizeof);
    //int result = ((int(*)())p)(60,9);

    memcpy(adder, code.ptr, code.length);
    //code.ptr.copy(adder);
    //adder = code.dup;
    //void* function(int, int) test = cast(void* function (int, int))adder;
    //int function(int, int) test = cast(int function (int, int))adder;
    //int x = test(60,9);
    //int i = *cast(int*)&test;
    //int result = i(60,9);

    int result = (cast(int function(int, int))adder)(60,9);
    writeln(result);

    munmap(adder, code.length);
    return result;
    //writeln("after munmap");
    //() @trusted { alloc.deallocate(p); p = null; }();
}
