const std = @import("std");

const STRIDE = 8;


fn buildLookupTable(comptime stride: usize) [1 << stride][stride]u8 {
    var lookupTable: [1 << stride][stride]u8 = undefined;
    const l: usize = (1 << stride) * stride;
    const max_loop_bounds: usize = if (1000 <= l) l + 1000 else 1000;
    @setEvalBranchQuota(max_loop_bounds);
    for (0..(1 << stride)) |mask| {
        var pos: u8 = 0;
        inline for (0..stride) |idx| {
            if (0 != (mask & (1 << idx))) {
                lookupTable[mask][pos] = idx;
                pos += 1;
            }
        }
    }
    return lookupTable;
}

fn filter(gpa: std.mem.Allocator, comptime stride: usize, item: u8, array_slice: []const u8) !std.ArrayList(usize) {
    const lookupTable: [1 << stride][stride]u8 = comptime buildLookupTable(stride);
    const item_vec: @Vector(stride, u8) = @splat(item);
    var count: usize = 0;
    var result: std.ArrayList(usize) = std.mem.zeroInit(std.ArrayList(usize), .{});
    errdefer result.deinit(gpa);
    while (array_slice.len >= count + stride) : (count += stride) {
        const temp: @Vector(stride, bool) = array_slice[count..][0..stride].* == item_vec;
        const mask: usize = @as(std.meta.Int(.unsigned, stride), @bitCast(temp));
        const matches: [stride]u8 = lookupTable[mask];
        const n: usize = @popCount(mask);
        for (0..n) |idx| try result.append(gpa, count + matches[idx]);
    }
    for (count..array_slice.len) |idx| {
        if (array_slice[idx] == item) try result.append(gpa, idx);
    }
    return result;
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    var test_list = [_]u8{7, 8, 5, 9, 2, 1, 2, 0};
    var result: std.ArrayList(usize) = try filter(allocator, STRIDE, 2, &test_list);
    defer result.deinit(allocator);
    std.debug.print("Here is the output: {any}\n", .{result.items});
}
