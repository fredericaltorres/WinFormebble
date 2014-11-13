/*
Sys.js
(c) 2011 TORRES Frederic
Freely distributable under the MIT license.

    - Works in a browser or in nodeJS
*/
var sys, Sys;
sys = (function(){
    var
        _sys = {};
        
    _sys.traversePath = function(obj, paths/*Array*/) {

         var o = obj;
         for(var i=0; i<paths.length; i++) {
            //console.log("get path:"+paths[i]);
            o = o[paths[i]];
         }
         return o;
    }

    _sys.distinct = function(arrayO, property) {
        var l = [];

        arrayO.forEach(function(e){
            var v = e[property];
            if(l.indexOf(v)===-1)
                l.push(v);
        });
        return l;
    }

    _sys.groupBy = function(arrayO, property) {
        var values  = sys.distinct(arrayO, property);
        var l       = { };
        
        values.forEach(function(v){
            l[v] = [];
        });
        arrayO.forEach(function(o){
            var cat = o[property];
            if(cat) {
                l[cat].push(o);
            }
        });
        return l;
    }
    ///
    /// Get rid of /* */ comment in text. But do not parse
    /// the content of the string to verify that /* is really a comment
    /// If we find a */ missing we just give up!    : <
    ///
    _sys.getRidOfStarComment = function(s) {

        var s1, s2, p2, p1;
        
        while(true) {
            p1 = s.indexOf("/*")
            if(p1 >= 0) {
                p2 = s.indexOf("*/", p1);
                if(p2 >= 0) {
                    s1 = s.substring(0, p1);
                    s2 = s.substring(p2+2);
                    s = s1 + s2;
                }
                else {
                    break; // Stop without signaling an error found some data that may contain /* in a string which is ok
                }
            }
            else break;
          }
        return s;
    }

   _sys.isNumeric = function (n) {

        return !isNaN(parseFloat(n)) && isFinite(n);
    }
   _sys.dumpObject = function(o) {

        var l = [];
        l.push("{");
        for(var k in o) {
            if(this.isFunction(o[k])) {
                l.push("    Function "+k);
            }
            else {
                l.push("    "+sys.getType(o[k]) + " " + k + " " + o[k]);
            }
        }
        l.push("}");
        return l.join("\n");
    }
    _sys.extend = function(destination, source) {

        for(var property in source)
            destination[property] = source[property];
        return destination;
    }
    _sys.extendMethods = function(destination, source) {

        for(var property in source)
            if(this.isFunction(source[property]))
                destination[property] = source[property];
        return destination;
    }
    _sys.extendProperties = function(destination, source) {

        for(var property in source)
            if(!this.isFunction(source[property]))
                destination[property] = source[property];
        return destination;
    }
    _sys.removeMethods = function(o) {

        var method = sys.getMethods(o);
        for(i=0; i<method.length; i++)
            delete o[method[i]]
    }
     _sys.getMethods = function(o) {
        var l = [];
        var keys = Object.keys(o);
        for(i=0; i<keys.length; i++)
            if(this.isFunction(o[keys[i]]))
                l.push(keys[i]);
        return l;
    }    
    _sys.getObjects = function(o) {
        var l = [];
        var keys = Object.keys(o);
        for(i=0; i<keys.length; i++)
            if(this.isObject(o[keys[i]]))
                l.push(keys[i]);
        return l;
    }

    _sys.isFunction = function(v) { return this.getType(v) === "Function";  }
    _sys.isString   = function(v) { return this.getType(v) === "String";    }
    _sys.isBoolean  = function(v) { return this.getType(v) === "Boolean";   }
    _sys.isNumber   = function(v) { return this.getType(v) === "Number";    }
    _sys.isDate     = function(v) { return this.getType(v) === "Date";      }
    _sys.isArray    = function(v) { return this.getType(v) === "Array";     }
    _sys.isObject   = function(v) { return this.getType(v) === "Object";    }
    _sys.isRegExp   = function(v) { return this.getType(v) === "RegExp";    }

    _sys.isInteger  = function(x) {

        if(this.isNumber(x)) {
            var y = parseInt(x);
            if (isNaN(y)) return false;
            return x===y && x.toString()==y.toString();
        }
    }

    _sys.getType = function(v) {

        var type;
        if (v === null)
            return "null";
        if (typeof v === 'undefined')
            return "undefined";
        if (typeof v === 'undefined')
            return "undefined";

        type = Object.prototype.toString.call(v);
        type = type.replace("[object ", "");
        type = type.replace("]", "");
        return type;
    }
    _sys.isMethod = function(m) {
        ///	<summary>
        /// return true if m is of type method/function
        ///	</summary>
        ///	<param name="m" type="Function">A variable</param>
        return this.isFunction(m);
    }
    _sys.isTypeDate = function(d) {
        ///	<summary>
        /// Returns true if d is of type date
        ///	</summary>
        ///	<param name="d" type="Date"></param>
        if(sys.isNullOrUndefined(d)){
            //console.log("isTypeDate called");
            return false;
        }
        return this.isMethod(d.getMonth) && this.isMethod(d.getFullYear);
    }
    _sys.hasMethod = function(o, m) {
        ///	<summary>
        /// Returns true if object o has a method m
        ///	</summary>
        ///	<param name="o" type="object">The object</param>
        ///	<param name="m" type="string">The method name as a string</param>
        return typeof (o[m]) === 'function';
    }
    _sys.isNull = function(v) {
        ///	<summary>
        /// Returns true if v is null
        ///	</summary>
        ///	<param name="v" type=""></param>
        return (v === null);
    }
    _sys.isUndefined = function(v) {
        ///	<summary>
        /// Returns true if v is undefined
        ///	</summary>
        ///	<param name="v" type=""></param>
        return (typeof v === "undefined");
    }
    _sys.isNullOrUndefined = function(v) {
        ///	<summary>
        /// Returns true if v is null or undefined
        ///	</summary>
        ///	<param name="v" type=""></param>
        return (v === null) || (typeof v === "undefined");
    }
    _sys.isEmpty = function(v) {
        ///	<summary>
        /// Returns true if v is null or undefined
        ///	</summary>
        ///	<param name="v" type=""></param>
        return (v === null) || (typeof v === "undefined") || (v === "");
    }
    _sys.ifUndefined = function(v, defaultVal) {
        ///	<summary>
        /// Returns v if v is defined. if v is undefined returns defaultVal
        ///	</summary>
        ///	<param name="v" type=""></param>
        ///	<param name="defaultVal" type="">The default value</param>
        return this.isUndefined(v) ? defaultVal : v;
    }
    _sys.ifNullOrUndefined = function(v, defaultVal) {
        ///	<summary>
        /// Returns v if v is defined. if v is undefined returns defaultVal
        ///	</summary>
        ///	<param name="v" type=""></param>
        ///	<param name="defaultVal" type="">The default value</param>
        var vv = sys.isNullOrUndefined(v) ? defaultVal : v;
        return vv;
    }
    _sys.ifDefined = function(v, defaultVal) {
        ///	<summary>
        /// Returns v if v is defined. if v is undefined returns defaultVal
        ///	</summary>
        ///	<param name="v" type=""></param>
        ///	<param name="defaultVal" type="">The default value</param>
        return (this.ifUndefined(v, defaultVal))
    }
    _sys.defaultValue = function(v, defaultVal) {
        ///	<summary>
        /// Returns v if v is defined. if v is undefined returns defaultVal
        ///	</summary>
        ///	<param name="v" type=""></param>
        ///	<param name="defaultVal" type="">The default value</param>
        return (this.ifUndefined(v, defaultVal))
    }
    _sys.isDefined = function(v) {
        ///	<summary>
        /// Returns true if v is defined
        ///	</summary>
        ///	<param name="" type="string"></param>
        return !this.isUndefined(v);
    }
    _sys.hasValue = function(v) {
        ///	<summary>
        /// Returns true if v not undefined, not null and not an empty string
        ///	</summary>
        ///	<param name="" type="string"></param>
        var
            s = "" + v;

        if (this.isNullOrUndefined(v)) return false;
        return s.length > 0;
    }
    _sys.Inherit = function(Super, Class) {
        ///	<summary>
        /// Make instance Class inherits from the Super
        ///	</summary>
        ///	<param name="Super" type="object">The base class</param>
        ///	<param name="Class" type="object">The instance</param>
        var
            method = null;
    
        if (!sys.isNullOrUndefined(Super)) {
    
            if (!sys.isNullOrUndefined(Super.prototype)) {
    
                for (var method in Super.prototype)
                    Class.prototype[method] = Super.prototype[method];
            }
            for (var method in Super)
                Class.prototype[method] = Super.prototype[method];
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    /// print
    ///////////////////////////////////////////////////////////////////////////////
    _sys.__consoleExists = function() {
        ///	<summary>
        /// Returns true if the console object is available, in IE and FireFox the console
        /// object is not always available
        ///	</summary>
        ///	<param name="" type="string"></param>
        var
            c;
        try {
            c = console;
            return true;
        }
        catch (ex) {
        }
        return false;
    }
    _sys.__print = function() {
        ///	<summary>
        /// bla bla
        ///	</summary>
        ///	<param name="" type="string"></param>
        var
            i = 0,
            r = null,
            values = [];
    
        for (i = 0; i < arguments.length; i++)
            values.push(arguments[i])

        r = values.join();
    
        if (__consoleExists())
            console.log(r);
    
        return r;
    }
    if (typeof (print) === undefined) { // Print is defined in IE
        print = __print;
    }
    _sys.isNodeJs = function() {
        ///	<summary>
        /// bla bla
        ///	</summary>
        ///	<param name="" type="string"></param>
        return (typeof require === "function" && typeof Buffer === "function" && typeof Buffer.byteLength === "function" && typeof Buffer.prototype !== "undefined" && typeof Buffer.prototype.write === "function");
    }
    _sys.isBrowser = function() {
        ///	<summary>
        /// bla bla
        ///	</summary>
        ///	<param name="" type="string"></param>
        return (typeof window !== undefined);
    }
    return _sys;
})();

Sys = sys;
