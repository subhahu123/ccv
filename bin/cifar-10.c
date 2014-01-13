#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>

unsigned int get_current_time()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char** argv)
{
	ccv_enable_default_cache();
	ccv_convnet_layer_param_t params[] = {
		{
			.type = CCV_CONVNET_CONVOLUTIONAL,
			.bias = 0,
			.sigma = 0.01,
			.input = {
				.matrix = {
					.rows = 31,
					.cols = 31,
					.channels = 3,
				},
			},
			.output = {
				.convolutional = {
					.rows = 5,
					.cols = 5,
					.channels = 3,
					.border = 2,
					.strides = 1,
					.count = 32,
				},
			},
		},
		{
			.type = CCV_CONVNET_MAX_POOL,
			.input = {
				.matrix = {
					.rows = 31,
					.cols = 31,
					.channels = 32,
				},
			},
			.output = {
				.pool = {
					.size = 3,
					.strides = 2,
					.border = 0,
				},
			},
		},
		{
			.type = CCV_CONVNET_LOCAL_RESPONSE_NORM,
			.input = {
				.matrix = {
					.rows = 15,
					.cols = 15,
					.channels = 32,
				},
			},
			.output = {
				.rnorm = {
					.size = 3,
					.kappa = 1,
					.alpha = 0.00005,
					.beta = 0.75,
				},
			},
		},
		{
			.type = CCV_CONVNET_CONVOLUTIONAL,
			.bias = 0,
			.sigma = 0.01,
			.input = {
				.matrix = {
					.rows = 15,
					.cols = 15,
					.channels = 32,
				},
			},
			.output = {
				.convolutional = {
					.rows = 5,
					.cols = 5,
					.channels = 32,
					.border = 2,
					.strides = 1,
					.count = 32,
				},
			},
		},
		{
			.type = CCV_CONVNET_AVERAGE_POOL,
			.input = {
				.matrix = {
					.rows = 15,
					.cols = 15,
					.channels = 32,
				},
			},
			.output = {
				.pool = {
					.size = 3,
					.strides = 2,
					.border = 0,
				},
			},
		},
		{
			.type = CCV_CONVNET_LOCAL_RESPONSE_NORM,
			.input = {
				.matrix = {
					.rows = 7,
					.cols = 7,
					.channels = 32,
				},
			},
			.output = {
				.rnorm = {
					.size = 3,
					.kappa = 1,
					.alpha = 0.00005,
					.beta = 0.75,
				},
			},
		},
		{
			.type = CCV_CONVNET_CONVOLUTIONAL,
			.bias = 0,
			.sigma = 0.01,
			.input = {
				.matrix = {
					.rows = 7,
					.cols = 7,
					.channels = 32,
				},
			},
			.output = {
				.convolutional = {
					.rows = 5,
					.cols = 5,
					.channels = 32,
					.border = 2,
					.strides = 1,
					.count = 64,
				},
			},
		},
		{
			.type = CCV_CONVNET_AVERAGE_POOL,
			.input = {
				.matrix = {
					.rows = 7,
					.cols = 7,
					.channels = 64,
				},
			},
			.output = {
				.pool = {
					.size = 3,
					.strides = 2,
					.border = 0,
				},
			},
		},
		{
			.type = CCV_CONVNET_FULL_CONNECT,
			.bias = 0,
			.sigma = 0.01,
			.input = {
				.matrix = {
					.rows = 3,
					.cols = 3,
					.channels = 64,
				},
				.node = {
					.count = 3 * 3 * 64,
				},
			},
			.output = {
				.full_connect = {
					.count = 3 * 3 * 32,
				},
			},
		},
		{
			.type = CCV_CONVNET_FULL_CONNECT,
			.bias = 0,
			.sigma = 0.01,
			.input = {
				.matrix = {
					.rows = 3 * 3 * 32,
					.cols = 1,
					.channels = 1,
				},
				.node = {
					.count = 3 * 3 * 32,
				},
			},
			.output = {
				.full_connect = {
					.count = 10,
				},
			},
		},
	};
	ccv_convnet_t* convnet = ccv_convnet_new(1, params, 9);
	assert(argc == 5);
	int num1 = atoi(argv[2]);
	int num2 = atoi(argv[4]);
	FILE* r1 = fopen(argv[1], "rb");
	FILE* r2 = fopen(argv[3], "rb");
	if (r1 && r2)
	{
		int i, j, k;
		unsigned char bytes[32 * 32 + 1];
		ccv_array_t* categorizeds = ccv_array_new(sizeof(ccv_categorized_t), num1, 0);
		for (k = 0; k < num1; k++)
		{
			fread(bytes, 32 * 32 + 1, 1, r1);
			int c = bytes[0];
			ccv_dense_matrix_t* a = ccv_dense_matrix_new(31, 31, CCV_32F | CCV_C3, 0, 0);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3] = bytes[j + i * 32 + 1] / 255.0 * 2 - 1;
			fread(bytes, 32 * 32, 1, r1);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3 + 1] = bytes[j + i * 32] / 255.0 * 2 - 1;
			fread(bytes, 32 * 32, 1, r1);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3 + 2] = bytes[j + i * 32] / 255.0 * 2 - 1;
			ccv_categorized_t categorized = ccv_categorized(c, a, 0);
			ccv_array_push(categorizeds, &categorized);
		}
		ccv_array_t* tests = ccv_array_new(sizeof(ccv_categorized_t), num2, 0);
		for (k = 0; k < num2; k++)
		{
			fread(bytes, 32 * 32 + 1, 1, r2);
			int c = bytes[0];
			ccv_dense_matrix_t* a = ccv_dense_matrix_new(31, 31, CCV_32F | CCV_C3, 0, 0);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3] = bytes[j + i * 32 + 1] / 255.0 * 2 - 1;
			fread(bytes, 32 * 32, 1, r2);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3 + 1] = bytes[j + i * 32] / 255.0 * 2 - 1;
			fread(bytes, 32 * 32, 1, r2);
			for (i = 0; i < 31; i++)
				for (j = 0; j < 31; j++)
					a->data.f32[(j + i * 31) * 3 + 2] = bytes[j + i * 32] / 255.0 * 2 - 1;
			ccv_categorized_t categorized = ccv_categorized(c, a, 0);
			ccv_array_push(tests, &categorized);
		}
		ccv_convnet_layer_train_param_t layer_params[10];
		memset(layer_params, 0, sizeof(layer_params));
		
		layer_params[0].w.decay = 0.005;
		layer_params[0].w.learn_rate = 0.00005;
		layer_params[0].w.momentum = 0.9;
		layer_params[0].bias.decay = 0;
		layer_params[0].bias.learn_rate = 0.0001;
		layer_params[0].bias.momentum = 0.9;

		layer_params[3].w.decay = 0.005;
		layer_params[3].w.learn_rate = 0.00005;
		layer_params[3].w.momentum = 0.9;
		layer_params[3].bias.decay = 0;
		layer_params[3].bias.learn_rate = 0.0001;
		layer_params[3].bias.momentum = 0.9;

		layer_params[6].w.decay = 0.005;
		layer_params[6].w.learn_rate = 0.00005;
		layer_params[6].w.momentum = 0.9;
		layer_params[6].bias.decay = 0;
		layer_params[6].bias.learn_rate = 0.0001;
		layer_params[6].bias.momentum = 0.9;

		layer_params[8].w.decay = 1;
		layer_params[8].w.learn_rate = 0.00005;
		layer_params[8].w.momentum = 0.9;
		layer_params[8].bias.decay = 0;
		layer_params[8].bias.learn_rate = 0.0001;
		layer_params[8].bias.momentum = 0.9;

		layer_params[9].dor = 0.5; 
		layer_params[9].w.decay = 1;
		layer_params[9].w.learn_rate = 0.00005;
		layer_params[9].w.momentum = 0.9;
		layer_params[9].bias.decay = 0;
		layer_params[9].bias.learn_rate = 0.0001;
		layer_params[9].bias.momentum = 0.9;

		ccv_convnet_train_param_t params = {
			.max_epoch = 999,
			.mini_batch = 128,
			.layer_params = layer_params,
		};
		ccv_convnet_supervised_train(convnet, categorizeds, tests, params);
	}
	if (r1)
		fclose(r1);
	if (r2)
		fclose(r2);
	ccv_convnet_free(convnet);
	ccv_disable_cache();
	return 0;
}