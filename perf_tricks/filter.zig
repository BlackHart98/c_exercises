const std = @import("std");

const STRIDE = 8;

pub const FilterResult = struct {
    out_slice: []usize, size: usize
};


fn buildLookupTable(comptime stride: usize) [1 << stride][stride]u8 {
    std.debug.assert(8 >= stride);
    var lookup_table: [1 << stride][stride]u8 = undefined;
    const l: usize = (1 << stride) * stride;
    const max_loop_bounds: usize = if (1000 <= l) l * 1000 else 1000;
    @setEvalBranchQuota(max_loop_bounds);
    for (0..(1 << stride)) |mask| {
        var pos: u8 = 0;
        inline for (0..stride) |idx| {
            if (0 != (mask & (1 << idx))) {
                lookup_table[mask][pos] = idx;
                pos += 1;
            }
        }
    }
    return lookup_table;
}

fn filter(
    comptime T: type,
    comptime stride: usize, 
    item: T, 
    array_slice: []const T,
    buf: []usize,
) []usize {
    const lookup_table: [1 << stride][stride]u8 = comptime buildLookupTable(stride);
    const item_vec: @Vector(stride, T) = @splat(item);
    var count: usize = 0;
    var out_len: usize = 0;
    var out_array: []usize = buf;
    while (array_slice.len >= count + stride) : (count += stride) {
        const temp: @Vector(stride, bool) = array_slice[count..][0..stride].* == item_vec;
        const mask: usize = @as(std.meta.Int(.unsigned, stride), @bitCast(temp));
        const matches: [stride]u8 = lookup_table[mask];
        const n: usize = @popCount(mask);
        for (0..n) |idx| {
            out_array[out_len] = count + matches[idx]; out_len += 1;
        }
    }
    const max: usize = @max(stride - 1, 2);
    switch (array_slice.len - count) {
        inline 2...max => |leftover_stride| {
            const lookup_table_: [1 << leftover_stride][leftover_stride]u8 = comptime buildLookupTable(leftover_stride);
            const item_vec_: @Vector(leftover_stride, u8) = @splat(item);
            const temp: @Vector(leftover_stride, bool) = array_slice[count..][0..leftover_stride].* == item_vec_;
            const mask: usize = @as(std.meta.Int(.unsigned, leftover_stride), @bitCast(temp));
            const matches: [leftover_stride]u8 = lookup_table_[mask];
            const n: usize = @popCount(mask);
            for (0..n) |idx| {
                out_array[out_len] = count + matches[idx]; out_len += 1;
            }
        },
        1 => if (array_slice[count] == item) {out_array[out_len] = count; out_len += 1;},
        else => {},
    }
    return out_array[0..out_len];
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    var test_list = [_]u8{ 
        7, 8, 5, 9, 2, 1, 2, 0, 3,
        6, 4, 5, 1, 7, 2, 9, 0, 5, 
        8, 3, 6, 1, 4, 7, 2, 5, 9,
        0, 3, 8, 1, 6, 4, 2, 7, 5, 
        9, 3, 0, 8, 1, 6, 4, 2, 7, 
        5, 9, 3, 0, 8 };
    const test_len = test_list.len;
    const buffer: []usize = try allocator.alloc(usize, test_len);
    defer allocator.free(buffer);
    const result: []usize = filter(u8, STRIDE, 5, &test_list, buffer);
    std.debug.print("Here is the output: {any}\n", .{result});
}
