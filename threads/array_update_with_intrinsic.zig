const std = @import("std");


pub fn main() u8 {
    const input_array: [9]i32 = .{1, 2, 3, 4, 5, 6, 7, 8, 9};
    var result_array: [9]i32 = @splat(@as(i32, 0));
    const event_array: [9]i32 = .{-1, -2, -1, -3, -1, -4, -1, -2, -1};

    _ = addIntrinsic(&input_array, &event_array, &result_array);

    for (result_array, 0..) |item, i| {
        std.debug.print("result_array[{}] : {}\n", .{i, item});
    }
    return 0;
}


fn addIntrinsic(input_array: []const i32, event_array: []const i32, result: []i32) u8 {
    std.debug.assert(input_array.len == event_array.len and result.len == event_array.len);
    var count: usize = 0;
    while ((count + 4) <= input_array.len) : (count += 4) {
        const l: @Vector(4, i32) = input_array[count..][0..4].*;
        const r: @Vector(4, i32) = event_array[count..][0..4].*;
        const sum = l + r;
        result[count..][0..4].* = sum;
    } 
    while (count < input_array.len) : (count += 1) {
        result[count] = input_array[count] + event_array[count];
    }
    return 1;
}
