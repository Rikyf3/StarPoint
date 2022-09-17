from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2
import tensorflow as tf

model = tf.keras.models.load_model('hopeless_diamond.h5')

full_model = tf.function(lambda inputs: model(inputs))
full_model = full_model.get_concrete_function([tf.TensorSpec(model_input.shape, model_input.dtype) for model_input in model.inputs])

frozen_func = convert_variables_to_constants_v2(full_model)

print(frozen_func.graph.as_graph_def())

tf.io.write_graph(graph_or_graph_def=frozen_func.graph, logdir="./frozen_models", name="frozen_graph.pb", as_text=False)
