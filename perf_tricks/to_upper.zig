const std = @import("std");

fn branchlessToUpper(comptime stride: usize, word: []u8) []u8 {
    const deduct: @Vector(stride, u8) = @splat(32);
    var count: usize = 0;
    var multiplier: @Vector(stride, u8) = @splat(0);
    while ((count + stride) < word.len) : (count += stride) {
        computeMultiplierUpper(stride, &multiplier, word, count);
        word[count..][0..stride].* = word[count..][0..stride].* - @as(@Vector(stride, u8), @intCast(multiplier * deduct));
    }
    for (count..word.len) |idx| {
        word[idx] -= 32 * @as(u8, @intFromBool('a' <= word[idx] and 'z' >= word[idx]));
    }
    return word;
}

inline fn computeMultiplierUpper(
    comptime stride: usize,
    multiplier: *@Vector(stride, u8),
    word: []u8, 
    offset: usize
) void {
    for (0..stride) |idx|{
        multiplier[idx] = @intFromBool('a' <= word[offset + idx] and 'z' >= word[offset + idx]);
    }
} 



pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const allocator = arena.allocator();
    const test_string: []const u8 = "Hello world! My name is blah blah blah.";
    std.debug.print("Here is the output: {s}\n", .{branchlessToUpper(8, try allocator.dupe(u8, test_string))});
}