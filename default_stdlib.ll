; target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"

%Render_Target = type { i32 }
%combined_image_t = type { i32 }
%image_t = type { i32 }
%sampler_t = type { i32 }

declare void @store_rt_2d_f4(%Render_Target* %0, <2 x i32> %coord, <4 x float> %val)
; separate sampler+image sample function
declare <4 x float> @sample_sepr_2d_f4(%sampler_t* %smp, %image_t *%img, <2 x float> %uv)
; combined sampler function
declare <4 x float> @sample_comb_2d_f4(%combined_image_t %cimg, <2 x float> %uv)
declare i8 addrspace(9) *@get_push_constant_ptr()
declare i8 addrspace(2) *@get_uniform_ptr(i32 %set, i32 %binding)
declare i8 addrspace(0) *@get_uniform_const_ptr(i32 %set, i32 %binding)
declare i8 addrspace(1) *@get_input_ptr(i32 %id)
declare i8 addrspace(3) *@get_output_ptr(i32 %id)