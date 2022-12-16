# StarPoint - Brings Star Reduction to the next level!

**StarPoint** is a convolutional neural network that given an image finds all the stars in it and models each star as a 2D gaussian profile.

## Why we need it?
In astrophotography star reduction is a tecnique widely used to reduce star impact on the image helping to bring out more structure and nebulosity. Typical tecniques require much trial and error and gives little control on star profile. Nonetheless they usually produce artifacts around stars.

![Untitled-1](https://user-images.githubusercontent.com/27781906/191260951-b5058191-9d39-4ea4-8a6c-922b0f15db1c.png)

Where others star reduction procedures try to erode stars using different types of filters, **StarPoint takes all of a different route**. At the cost of an increase in computational power used, every single star in an image is modelled as a gaussian profile, giving access to modifily not only variance along each axis, but also rotation, position and brightness of each star independentely. This makes possible not only to reduce star size, but also to remove cromatic aberration and correct small tracking error that creates not perfectly round stars.

## Usage in PixInsight
StarPoint takes a star mask and returns a modified version of it.

> Star amplitude : a multiplicative constant applied to every star amplitude.<br />
> If greater than 1 it increases star brightness, otherwise it reduces it.

> Deviation multiplier : a multiplicative constant applied to every star variance.<br />
> The effect on the image is a reduction of star sizes.

## Self-supervision: a cost effective approach to AI
In contrary to many other method of traning a neural network StarPoint uses a tecnique called **self-supervision**. The traning dataset of StarPoint is completely synthetic. For every training step a number of gaussian parameters (variance, rotation, amplitude, position, ...) are generated randomly. The input of the CNN is the renderning of that paramenters onto images and the output is the parameters themselves.
This approach does not require any pre existing dataset of images reducing the overall cost and time spent. Another advantage is that if an issue occurs, for example star of big size are not correctly parametrized by the AI, it is quite easy to solve the problem.

## I hate AI in astrophotography too!
Traditional techniques (in the sense of non-AI algorithms) operate by mathematical priors, not by memorization of previously seen data. This means that how good the outcome of the algorithm is does not depend on the similarity of the input image with the training data. In simpler terms artifacts in non-AI algorithms are a consequence of the input image, but artifacts in AI algorithms are both consequence of input image and training data. This means that AI tends to work very well in an already seen context and to work poorly in other cases. Does StarPoint solve this problem? Absolutely not! StarPoint still utilizes AI. The sole difference with other AI solutions resides in the use of a completely synthetic training set. This is possible because we have a complete mathematical understanding of how lights behave in an optical system. StarPoint training dataset is potentially infinite and considers every case possible. No other AI algorithm released can do that.
