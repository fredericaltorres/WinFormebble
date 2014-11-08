/*
string.js

(c) 2011 TORRES Frederic
Freely distributable under the MIT license.
    - Works in a browser or in nodeJS
    - All the methods are extension method of the class string
*/


String.prototype.decapitalize = function(  ) {
    ///	<summary>
    /// Convert a camelCase PascalCase or underscore programming id into caption or phrase
    /// Sample 
    ///     "ThisIsABigID".decapitalize() => "This Is A Big ID"
    ///     "StateInTheUSA".decapitalize() => "State In The USA"
    ///     "state_in_the_USA".decapitalize() => "State In The USA"
    ///	</summary>
    var text = this;
    var finalResult = text.
                        replace(/([A-Z]+)/g, " $1").        // Insert a space before any upper case -> replace(/([A-Z]+)/g, " $1") => "In The USAAnd Else"
                        replace(/([A-Z][a-z])/g, " $1").    // Convert  "In The USAAnd Else" =>  "In The USA And Else"
                        replace(/_/g, " ").                 // Support Id separated with '_'. A la Ruby
                        replace(/-/g, " ");                 // Support Id separated with '_'. A la Ruby

    while(finalResult.indexOf("  ")!==-1) 
        finalResult = finalResult.replace(/  /g, " ");

    finalResult = finalResult.trim(); // Some side effect of above code add space at the beginning

    // Support this is an ID => This Is An ID - Ruby id
    finalResult = finalResult.replace(/(\S)(\S*)/g, function($0,$1,$2){
        return $1.toUpperCase()+$2;
    });
    return finalResult;
}


///////////////////////////////////////////////////////////////////////////////
/// String Extensions
///////////////////////////////////////////////////////////////////////////////
String.prototype.remove = function( idx, count ) {

    if(typeof(count)==='undefined'){
        count = this.length;
    }
    return this.slice(0,idx) + this.slice(idx+count);
}
String.prototype.insert = function( idx, s ) {

    return (this.slice(0,idx) + s + this.slice(idx));
}
String.prototype.capitalize = function () {
    ///	<summary>
    /// Capitalize the string, 
    /// by removing the space and changing the first letter of each word to an uppercase
    ///	</summary>
    return this.charAt(0).toUpperCase() + this.slice(1).toLowerCase();
};
String.prototype.leftPad = function (padString, length) {
    ///	<summary>
    /// Pad the string on the left    
    ///	</summary>
    ///	<param name="padString" type="string">The character to use for the padding</param>
    ///	<param name="length" type="number">The max size of the string to reach</param>
    var str = this;
    while (str.length < length)
        str = padString + str;
    return str;
}
String.prototype.rightPad = function (padString, length) {
    ///	<summary>
    /// Pad the string on the right
    ///	</summary>
    ///	<param name="padString" type="string">The character to use for the padding</param>
    ///	<param name="length" type="number">The max size of the string to reach</param>
    var str = this;
    while (str.length < length)
        str = str + padString;
    return str;
}
String.prototype.trim = function () {
    ///	<summary>
    /// Trim the string on the left and on the right
    ///	</summary>
    return this.replace(/^\s+|\s+$/g, "");
}
String.prototype.leftTrim = function () {
    ///	<summary>
    /// Trim the string on the left
    ///	</summary>
    return this.replace(/^\s+/, "");
}
String.prototype.rightTrim = function () {
    ///	<summary>
    /// Trim the string on the right
    ///	</summary>
    return this.replace(/\s+$/, "");
}
String.prototype.contains = function (str) {
    ///	<summary>
    /// Return true if the string contains a specific sub string
    ///	</summary>
    ///	<param name="str" type="string">The string to search</param>
    var 
        pos = this.indexOf(str);
    return pos !== -1;
}
String.prototype.replaceAll = function (searched, replaced) {
    ///	<summary>
    /// Replace all instance of searched with replaced
    ///	</summary>
    ///	<param name="searched" type="string">The string to search</param>
    ///	<param name="replaced" type="string">The string to replaced</param>
    var s = this;
    while(s.contains(searched)) {
         s = s.replace(searched, replaced);
    }
    return s;
}
String.prototype.startsWith = function (str) { 
    ///	<summary>
    /// Return true if the string start with a specific string
    ///	</summary>
    ///	<param name="str" type="string">The string to start with</param>
    return (this.indexOf(str) === 0); 
}
String.prototype.endsWith = function (str) {
    ///	<summary>
    /// Return true if the string end with a specific string
    ///	</summary>
    ///	<param name="str" type="string">The string to end with</param>
    var lastIndex = this.lastIndexOf(str);
    return (lastIndex != -1) && (lastIndex + str.length == this.length);
}
///////////////////////////////////////////////////////////////////////////////
// format()
//
// Usage:
//      "LastName:{lastName}, Age:{Age}".format({ lastName:"TORRES", Age:45 });
//
//      "LastName:{0}, Age:{1}".format("TORRES", 45);
//
String.prototype.format = function () {
    ///	<summary>
    ///Format the string based on a list of argument referenced in the format template&#10;
    ///with the synatx {index} or format the string based on the format template referencing&#10;
    ///propertu name of the instance with the syntax {property-name}&#10;
    ///Sample:&#10;
    ///     var r = "LastName:{lastName}, Age:{Age}".format({ lastName:"TORRES", Age:45 });&#10;
    ///     var r = "LastName:{0}, Age:{1}".format("TORRES", 45);&#10;
    ///	</summary>
    ///	<param name="o" type="object">The object feeding the template via its properties</param>
    ///	<param name="p1" type="object">value</param>
    var
        stringValue = this;

    // Pass one instance
    if ((arguments.length === 1) && (typeof arguments[0] === "object") && (!Array.isArray(arguments[0]))) {

        for (var i = 0; i < arguments.length; i++) {
            var instance = arguments[i];
            for (var p in instance) {
                stringValue = stringValue.replace(new RegExp('{' + p + '}', 'g'), instance[p]);
            }
        }
    }
    else {
        if (stringValue.indexOf("{") == -1)
            throw new Error("format string '" + stringValue + "' should contains at least one parameters");

        for (var i = 0; i < arguments.length; i++) {

            var v = arguments[i];
            if (Array.isArray(v)) // format the array to a string
                if(v.format)
                    v = v.format();
            stringValue = stringValue.replace(new RegExp('\\{' + (i) + '\\}', 'gm'), v);
        }
    }
    return stringValue;
}
String.format = function () {
    ///	<summary>
    ///Format the string passed as first argument based on the list of following arguments referenced in the format template&#10;
    ///with the synatx {index} 
    ///Sample:&#10;
    ///     var r = "LastName:{0}, Age:{1}".format("TORRES", 45);&#10;
    ///	</summary>    
    ///	<param name="tpl" type="string">value</param>
    ///	<param name="p1" type="object">value</param>
    for (var i = 1; i < arguments.length; i++)
        arguments[0] = arguments[0].replace(new RegExp('\\{' + (i - 1) + '\\}', 'gm'), arguments[i]);
    return arguments[0];
}
