ANH Utilities Library
=====================

The ANH utilities library is a collection of classes and helper functions that are common to (nearly) all applications developed by ANH. These utilities are intended to be used in conjunction with the c++ standard library and boost libraries to assist in common application tasks.

## Utilities Overview ##

The following is a high-level overview of the utilities found in the ANH Utilities Library. For further information see the [project documentation][1] or browse the [online API][2].

### Active Object ###

There are many times when it makes sense to break an object off and run it concurrently while the rest of the application runs. The ActiveObject is a reusable facility that encourages the encapsulation of data by using asynchronus messages to process requests in a private thread. This implementation is based on [a design][3] discussed by [Herb Sutter][4].


  [1]: http://github.com/anhstudios/utilities/wiki
  [2]: http://projects.anhstudios.com/utilities/api
  [3]: http://www.drdobbs.com/go-parallel/article/showArticle.jhtml?articleID=225700095
  [4]: http://herbsutter.com/
