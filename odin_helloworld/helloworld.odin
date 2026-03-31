package helloworld


import "core:fmt"
import "core:strings"


branchless_to_upper :: proc($stride: int, str: []u8) {
    count: int = 0;
    for count + stride < len(str) {
        deduct: [stride]u8 = [?]u8{0..<stride = 32};
        multiplier: [stride]u8 = [?]u8{0..<stride = 0};
        #unroll for idx in 0..<stride {
            multiplier[idx] = cast(u8)(('a' <= str[count + idx]) && ('z' >= str[count + idx]))
        }
        #unroll for idx in 0..<stride {
            str[count + idx] -= multiplier[idx] * deduct[idx]
        }
        count += stride
    }
    for idx in count..<len(str) {
        str[idx] -= 32 * cast(u8)(('a' <= str[idx]) && ('z' >= str[idx]));
    }
} 


branchless_to_lower :: proc($stride: int, str: []u8) {
    count: int = 0;
    for count + stride < len(str) {
        deduct: [stride]u8 = [?]u8{0..<stride = 32};
        multiplier: [stride]u8 = [?]u8{0..<stride = 0};
        #unroll for idx in 0..<stride {
            multiplier[idx] = cast(u8)(('A' <= str[count + idx]) && ('Z' >= str[count + idx]))
        }
        #unroll for idx in 0..<stride {
            str[count + idx] += multiplier[idx] * deduct[idx]
        }
        count += stride
    }
    for idx in count..<len(str) {
        str[idx] += 32 * cast(u8)(('A' <= str[idx]) && ('Z' >= str[idx]));
    }
}

main :: proc() {
    test_string:string: 
`In a quiet town where the evenings arrive slowly and the wind moves gently
through the trees, a curious programmer sits at a desk illuminated by the
pale glow of a monitor, thinking about compilers, memory layouts, and the
strange beauty of systems that transform symbols into meaning, while
somewhere in the background a kettle hums softly and the world outside
continues its steady rhythm, cars passing occasionally, distant
conversations fading into the night, and yet inside this small room a
different universe unfolds, one made of bits and instructions and
structures carefully arranged so that ideas may travel from the mind into
code and from code into machines that obey with perfect precision, and as
the programmer types line after line the sentence grows longer and longer,
weaving together fragments of observation, reflection, and imagination
until it becomes an unusually extended thought that stretches far beyond
what anyone would normally consider a reasonable sentence, continuing
through reflections on persistence, quiet curiosity, and the peculiar joy
that appears when a difficult system finally behaves exactly as intended.`;
    result: string = strings.clone(test_string)
    defer delete(result)
    branchless_to_upper(4, transmute([]u8) result)
    fmt.println("Here is the output:\n", result);
}