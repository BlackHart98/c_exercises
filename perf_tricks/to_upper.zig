const std = @import("std");

fn branchlessToUpper(comptime stride: usize, word: []u8) []u8 {
    var count: usize = 0;
    while ((count + stride) <= word.len) : (count += stride) {
        const deduct: @Vector(stride, u8) = @splat(32);
        var multiplier: @Vector(stride, u8) = @splat(0);
        computeMultiplierUpper(stride, &multiplier, word, count);
        word[count..][0..stride].* = word[count..][0..stride].* - @as(@Vector(stride, u8), @intCast(multiplier * deduct));
    }
    switch (word.len - count) {
        inline 2...@max(stride - 1, 2) => |leftover_stride| {
            const deduct: @Vector(leftover_stride, u8) = @splat(32);
            var multiplier: @Vector(leftover_stride, u8) = @splat(0);
            computeMultiplierUpper(leftover_stride, &multiplier, word, count);
            word[count..][0..leftover_stride].* = word[count..][0..leftover_stride].* - @as(@Vector(leftover_stride, u8), @intCast(multiplier * deduct));
        },
        1 => {word[count] -= 32 * @as(u8, @intFromBool('a' <= word[count] and 'z' >= word[count]));},
        else => {},
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
    const test_string =
        \\In a quiet town where the evenings arrive slowly and the wind moves gently
        \\through the trees, a curious programmer sits at a desk illuminated by the
        \\pale glow of a monitor, thinking about compilers, memory layouts, and the
        \\strange beauty of systems that transform symbols into meaning, while
        \\somewhere in the background a kettle hums softly and the world outside
        \\continues its steady rhythm, cars passing occasionally, distant
        \\conversations fading into the night, and yet inside this small room a
        \\different universe unfolds, one made of bits and instructions and
        \\structures carefully arranged so that ideas may travel from the mind into
        \\code and from code into machines that obey with perfect precision, and as
        \\the programmer types line after line the sentence grows longer and longer,
        \\weaving together fragments of observation, reflection, and imagination
        \\until it becomes an unusually extended thought that stretches far beyond
        \\what anyone would normally consider a reasonable sentence, continuing
        \\through reflections on persistence, quiet curiosity, and the peculiar joy
        \\that appears when a difficult system finally behaves exactly as intended.
    ;

    std.debug.print("Here is the output: {s}\n", .{branchlessToUpper(16, try allocator.dupe(u8, test_string))});
}