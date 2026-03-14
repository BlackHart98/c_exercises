const std = @import("std");

const STRIDE = 4;


// Stride capped at 4 element stride, I have to figure out how to fix this to make it possible to 
fn buildLookupTable(comptime stride: u8) [1 << stride][stride]u8 {
    std.debug.assert(4 >= stride);
    var lookupTable: [1 << stride][stride]u8 = undefined;
    for (0..(1 << stride)) |mask| {
        var pos: u8 = 0;
        inline for (0..stride) |idx| {
            if (0 != (mask & (1 << idx))){
                lookupTable[mask][pos] = idx;
                pos += 1;
            }
        }
    }
    return lookupTable;
}

fn filter(gpa: std.mem.Allocator, comptime stride: usize, item: u8, array_slice: []u8) !std.ArrayList(usize) {
    const lookupTable = comptime buildLookupTable(stride);
    const item_vec: @Vector(stride, u8) = @splat(item);
    var count: usize = 0;
    var result: std.ArrayList(usize) = std.mem.zeroInit(std.ArrayList(usize), .{});
    errdefer result.deinit(gpa);
    while (array_slice.len >= count + stride) : (count += stride) {
        const temp: @Vector(stride, bool) = array_slice[count..][0..stride].* == item_vec;
        var mask: usize = 0;
        inline for (0..stride) |idx| mask |= @as(usize, @intFromBool(temp[idx])) << idx;
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

    var test_list = [8]u8{7, 8, 5, 9, 2, 1, 2, 0};
    var result: std.ArrayList(usize) = try filter(allocator, STRIDE, 2, &test_list);
    defer result.deinit(allocator);
    std.debug.print("Here is the output: {any}\n", .{result.items});
}
