package io.ionic.backgroundrunner.android_engine

class JSValue {
    val isBool: Boolean = false
    val isNumber: Boolean = false
    val isString: Boolean = false
    val isNull: Boolean = false
    val isUndefined: Boolean = false
    val isFunction: Boolean = false
    val isArray: Boolean = false

    private val doubleValue: Double? = null;
    private val boolValue: Boolean? = null;
    private val strValue: String? = null;

    fun getIntValue(): Int? {
        return doubleValue?.toInt()
    }

    fun getFloatValue(): Float? {
        return doubleValue?.toFloat()
    }

    fun getStringValue(): String? {
        return strValue
    }

    fun getBoolValue(): Boolean? {
        return boolValue;
    }
}