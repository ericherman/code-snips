/*
   gcc -Wall -Wextra -Werror -Wpedantic \
	-o opencl-vector-func \
	opencl-vector-func.c \
	-lOpenCL

   modified from:
   https://github.com/smistad/OpenCL-Getting-Started/blob/master/main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
/* ocl-icd-libopencl1, opencl-headers */
#include <CL/cl.h>
#endif

#define assert_msg(expr, msg) \
	do { if (!(expr)) { \
		fprintf(stderr, "%s:%d ERROR (%s) '%s'\n", \
			__FILE__, __LINE__, #expr, msg); \
		exit(EXIT_FAILURE); } \
	} while (0)

#define assert_long_equal_msg(actual, expected, msg) \
	do { if ((actual) != (expected)) { \
		fprintf(stderr, "%s:%d ERROR (%ld != %ld) '%s'\n", \
			__FILE__, __LINE__, \
			(long)(actual), (long)(expected), msg); \
		exit(EXIT_FAILURE); } \
	} while (0)

char *opencl_vector_float_func_src =	/*                these comments  */
    "__kernel void vector_float_func(\n"	/*        are just to     */
    "\t\t__global float *input_a,\n"	/*                keep the code   */
    "\t\t__global float *input_b,\n"	/*                auto formatter  */
    "\t\t__global float *result)\n"	/*                from putting    */
    "{\n"			/*                        all the strings */
    "\tint i = get_global_id(0);\n"	/*                together on two */
    "\tresult[i] = input_a[i] * input_b[i];\n"	/*        or three lines  */
    "}\n";			/*    I wish C had multi-line strings.... */

char *slurp_file_to_string(const char *file_path, size_t *buf_size)
{
	FILE *fp;
	struct stat st;
	size_t size;
	char *str;

	if (buf_size) {
		*buf_size = 0;
	}
	str = NULL;
	stat(file_path, &st);
	size = (size_t)st.st_size;
	if (size == SIZE_MAX) {
		return NULL;
	}

	fp = fopen(file_path, "r");
	if (!fp) {
		return NULL;
	}

	/* add one to ensure speace for a null termination */
	str = malloc(size + 1U);
	if (!str) {
		fclose(fp);
		return NULL;
	}
	if (buf_size) {
		*buf_size = size + 1U;
	}

	if (size != fread(str, 1U, size, fp)) {
		free(str);
		str = NULL;
		if (buf_size) {
			*buf_size = 0;
		}
	}
	str[size] = '\0';

	fclose(fp);
	return str;
}

int main(int argc, char **argv)
{
	const size_t List_len = 1024U;
	size_t array_bytes_len = List_len * sizeof(float);

	float *input_a, *input_b, *result;
	size_t i, offset, buf_size, src_len;
	size_t global_item_size, process_in_groups_of;
	char *src_str = NULL;

	cl_uint num_entries, num_devices;
	cl_uint ret_num_devices, ret_num_platforms;
	cl_int ret;
	cl_command_queue_properties q_props;
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_mem a_mem_obj = NULL;
	cl_mem b_mem_obj = NULL;
	cl_mem res_mem_obj = NULL;

	/* Create the two local input arrays and a local output array */
	input_a = malloc(array_bytes_len);
	input_b = malloc(array_bytes_len);
	result = calloc(1, array_bytes_len);
	assert_msg((input_a && input_b && result), "allocating arrays");

	/* initialize input arrays with some arbitrary values */
	for (i = 0; i < List_len; i++) {
		input_a[i] = 1.0 * i;
		input_b[i] = List_len / (1.0 + i);
	}

	buf_size = 0;
	if (argc > 1) {
		src_str = slurp_file_to_string(argv[1], &buf_size);
	}
	if (src_str && buf_size > 1) {
		printf("using %s\n", argv[1]);
	} else {
		printf("using opencl_vector_float_func_src\n");
		src_str = opencl_vector_float_func_src;
		if (buf_size) {
			free(src_str);
			buf_size = 0;
		}
	}
	src_len = strlen(src_str);
	printf("program source:\n\n%s\nprogram length: %lu\n", src_str,
	       (unsigned long)src_len);
	fflush(stdout);

	/* Get platform and device information */
	num_entries = 1;
	ret = clGetPlatformIDs(num_entries, &platform_id, &ret_num_platforms);
	assert_long_equal_msg(ret, CL_SUCCESS, "clGetPlatformIDs");

	ret =
	    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, num_entries,
			   &device_id, &ret_num_devices);
	assert_long_equal_msg(ret, CL_SUCCESS, "clGetDeviceIDs");

	/* Create an OpenCL context */
	num_devices = 1;
	context =
	    clCreateContext(NULL, num_devices, &device_id, NULL, NULL, &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "clCreateContext");
	assert_msg((context != NULL), "the context obj");

	/* Create a command queue (this is deprecated 1.2 stuff) */
	q_props = 0;		/* bit-field, see man 3 clCreateCommandQueue */
	command_queue = clCreateCommandQueue(context, device_id, q_props, &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "clCreateCommandQueue");
	assert_msg((command_queue != NULL), "the command_queue obj");

	/* Create memory buffers on the device for each local vector */
	a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, array_bytes_len,
				   NULL, &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "a_mem_obj = clCreateBuffer");
	assert_msg((a_mem_obj != NULL), "a_mem_obj");

	b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, array_bytes_len,
				   NULL, &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "b_mem_obj = clCreateBuffer");
	assert_msg((b_mem_obj != NULL), "b_mem_obj");

	res_mem_obj =
	    clCreateBuffer(context, CL_MEM_WRITE_ONLY, array_bytes_len, NULL,
			   &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "res_mem_obj = clCreateBuffer");
	assert_msg((res_mem_obj != NULL), "res_mem_obj");

	/* Copy the lists input_a and input_b to their buffers */
	offset = 0;
	ret =
	    clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, offset,
				 array_bytes_len, input_a, 0, NULL, NULL);
	assert_long_equal_msg(ret, CL_SUCCESS, "clEnqueueWriteBuffer(q, a_mem");

	ret =
	    clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, offset,
				 array_bytes_len, input_b, 0, NULL, NULL);
	assert_long_equal_msg(ret, CL_SUCCESS, "clEnqueueWriteBuffer(q, b_mem");

	/* Create a program from the kernel source */
	program = clCreateProgramWithSource(context, 1, (const char **)&src_str,
					    (const size_t *)&src_len, &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "clCreateProgramWithSource");
	assert_msg((program != NULL), "program obj");
	if (buf_size) {
		free(src_str);
		buf_size = 0;
	}

/*
	ret = clGetProgramBuildInfo(program, &device_id,
                                    cl_program_build_info param_name,
                                    size_t param_value_size,
                                    void *param_value,
                                    size_t *param_value_size_ret);
*/

	/* Build the program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	assert_long_equal_msg(ret, CL_SUCCESS, "clBuildProgram");

	/* Create the OpenCL kernel */
	kernel = clCreateKernel(program, "vector_float_func", &ret);
	assert_long_equal_msg(ret, CL_SUCCESS, "kernel = clCreateKernel");
	assert_msg((kernel != NULL), "kernel obj");

	/* Set the arguments of the kernel */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	assert_long_equal_msg(ret, CL_SUCCESS, "clSetKernelArg( &a_mem_obj");

	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	assert_long_equal_msg(ret, CL_SUCCESS, "clSetKernelArg( &b_mem_obj");

	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&res_mem_obj);
	assert_long_equal_msg(ret, CL_SUCCESS, "clSetKernelArg( &res_mem_obj");

	/* Execute the OpenCL kernel on the list */
	global_item_size = List_len;	/* Process the entire lists */
	process_in_groups_of = 64;	/* Process in groups of 64 */
	ret =
	    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
				   &global_item_size, &process_in_groups_of, 0,
				   NULL, NULL);
	assert_long_equal_msg(ret, CL_SUCCESS, "clEnqueueNDRangeKernel");

	/* Read the memory buffer result on the device to the local result */
	ret =
	    clEnqueueReadBuffer(command_queue, res_mem_obj, CL_TRUE, 0,
				array_bytes_len, result, 0, NULL, NULL);
	assert_long_equal_msg(ret, CL_SUCCESS, "clEnqueueReadBuffer");

	/* Display the local result to the screen */
	for (i = 0; i < List_len; i++) {
		printf("%g, %g = %g\n", input_a[i], input_b[i], result[i]);
	}
	fflush(stdout);

	/* Clean up */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(a_mem_obj);
	ret = clReleaseMemObject(b_mem_obj);
	ret = clReleaseMemObject(res_mem_obj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(input_a);
	free(input_b);
	free(result);

	return 0;
}
