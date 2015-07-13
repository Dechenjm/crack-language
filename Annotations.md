# Annotations #

Annotations are Crack's mechanism for extending the compiler with the language.

An annotations consists of the at sign ("@") followed by an identifier.  The identifier can either refer to a function or a variable in another module.  If it refers to a function, the function should have the following signature:

```
void annotationFunction(CrackContext context);
```

`CrackContext` is a class that can be imported from the internal `crack.compiler` module.  It is a proxy interface for the internals of the compiler, giving you access to the tokenizer and the parser.

If the annotation variable is a

Annotations can occur anywhere in the token stream of your code (they are separate tokens, so if they occur in comments xxx finish xxx


# Details #

Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages