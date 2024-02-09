package com.ipvsserv.nxd.mdc.util;

import java.text.DateFormat;
import java.text.Format;
import java.text.MessageFormat;
import java.text.NumberFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.StringTokenizer;
import java.util.TimeZone;
import java.util.concurrent.ConcurrentHashMap;

import com.ipvsserv.common.config.IpvsServConstants;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class LocaleUtils {
    
    private static final Map<Locale, String[][]> timeZoneLists = new ConcurrentHashMap<Locale, String[][]>();
    private static final String resourceBaseName = "ipvscomponent_i18n";// "openfire_i18n";
    
    private LocaleUtils() {}
    
    public static Locale localeCodeToLocale(String localeCode) {
        Locale locale = null;
        if (localeCode != null) {
            String language = null;
            String country = null;
            String variant = null;
            StringTokenizer tokenizer = new StringTokenizer(localeCode, "_");
            if (tokenizer.hasMoreTokens()) {
                language = tokenizer.nextToken();
                if (tokenizer.hasMoreTokens()) {
                    country = tokenizer.nextToken();
                    if (tokenizer.hasMoreTokens()) {
                        variant = tokenizer.nextToken();
                    }
                }
            }
            locale = new Locale(language, (country != null ? country : ""), (variant != null ? variant : ""));
        }
        return locale;
    }
    
    // The list of supported timezone ids. The list tries to include all of the relevant
    // time zones for the world without any extraneous zones.
    private static String[] timeZoneIds = new String[] { "GMT", "Pacific/Apia", "HST", "AST", "America/Los_Angeles", "America/Phoenix", "America/Mazatlan", "America/Denver", "America/Belize", "America/Chicago", "America/Mexico_City", "America/Regina", "America/Bogota", "America/New_York", "America/Indianapolis", "America/Halifax", "America/Caracas", "America/Santiago", "America/St_Johns", "America/Sao_Paulo", "America/Buenos_Aires", "America/Godthab", "Atlantic/South_Georgia", "Atlantic/Azores", "Atlantic/Cape_Verde", "Africa/Casablanca", "Europe/Dublin", "Europe/Berlin", "Europe/Belgrade", "Europe/Paris", "Europe/Warsaw", "ECT", "Europe/Athens", "Europe/Bucharest", "Africa/Cairo", "Africa/Harare", "Europe/Helsinki", "Asia/Jerusalem", "Asia/Baghdad", "Asia/Kuwait", "Europe/Moscow", "Africa/Nairobi", "Asia/Tehran", "Asia/Muscat", "Asia/Baku", "Asia/Kabul", "Asia/Yekaterinburg", "Asia/Karachi", "Asia/Calcutta", "Asia/Katmandu", "Asia/Almaty", "Asia/Dhaka", "Asia/Colombo", "Asia/Rangoon", "Asia/Bangkok", "Asia/Krasnoyarsk", "Asia/Hong_Kong", "Asia/Irkutsk", "Asia/Kuala_Lumpur", "Australia/Perth", "Asia/Taipei", "Asia/Tokyo", "Asia/Seoul", "Asia/Yakutsk", "Australia/Adelaide", "Australia/Darwin", "Australia/Brisbane", "Australia/Sydney", "Pacific/Guam", "Australia/Hobart", "Asia/Vladivostok", "Pacific/Noumea", "Pacific/Auckland", "Pacific/Fiji", "Pacific/Tongatapu" };
    
    // A mapping from the supported timezone ids to friendly english names.
    private static final Map<String, String> nameMap = new HashMap<String, String>();
    
    static {
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[0], "International Date Line West");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[1], "Midway Island, Samoa");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[2], "Hawaii");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[3], "Alaska");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[4], "Pacific Time (US & Canada); Tijuana");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[5], "Arizona");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[6], "Chihuahua, La Pax, Mazatlan");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[7], "Mountain Time (US & Canada)");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[8], "Central America");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[9], "Central Time (US & Canada)");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[10], "Guadalajara, Mexico City, Monterrey");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[11], "Saskatchewan");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[12], "Bogota, Lima, Quito");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[13], "Eastern Time (US & Canada)");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[14], "Indiana (East)");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[15], "Atlantic Time (Canada)");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[16], "Caracas, La Paz");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[17], "Santiago");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[18], "Newfoundland");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[19], "Brasilia");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[20], "Buenos Aires, Georgetown");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[21], "Greenland");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[22], "Mid-Atlantic");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[23], "Azores");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[24], "Cape Verde Is.");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[25], "Casablanca, Monrovia");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[26], "Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[27], "Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[28], "Belgrade, Bratislava, Budapest, Ljubljana, Prague");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[29], "Brussels, Copenhagen, Madrid, Paris");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[30], "Sarajevo, Skopje, Warsaw, Zagreb");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[31], "West Central Africa");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[32], "Athens, Istanbul, Minsk");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[33], "Bucharest");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[34], "Cairo");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[35], "Harare, Pretoria");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[36], "Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[37], "Jerusalem");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[38], "Baghdad");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[39], "Kuwait, Riyadh");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[40], "Moscow, St. Petersburg, Volgograd");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[41], "Nairobi");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[42], "Tehran");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[43], "Abu Dhabi, Muscat");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[44], "Baku, Tbilisi, Muscat");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[45], "Kabul");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[46], "Ekaterinburg");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[47], "Islamabad, Karachi, Tashkent");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[48], "Chennai, Kolkata, Mumbai, New Dehli");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[49], "Kathmandu");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[50], "Almaty, Novosibirsk");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[51], "Astana, Dhaka");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[52], "Sri Jayawardenepura");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[53], "Rangoon");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[54], "Bangkok, Hanoi, Jakarta");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[55], "Krasnoyarsk");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[56], "Beijing, Chongqing, Hong Kong, Urumqi");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[57], "Irkutsk, Ulaan Bataar");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[58], "Kuala Lumpur, Singapore");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[59], "Perth");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[60], "Taipei");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[61], "Osaka, Sapporo, Tokyo");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[62], "Seoul");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[63], "Yakutsk");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[64], "Adelaide");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[65], "Darwin");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[66], "Brisbane");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[67], "Canberra, Melbourne, Sydney");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[68], "Guam, Port Moresby");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[69], "Hobart");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[70], "Vladivostok");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[71], "Magadan, Solomon Is., New Caledonia");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[72], "Auckland, Wellington");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[73], "Fiji, Kamchatka, Marshall Is.");
        LocaleUtils.nameMap.put(LocaleUtils.timeZoneIds[74], "Nuku'alofa");
    }
    
    public static String[][] getTimeZoneList() {
        Locale jiveLocale = IpvsServGlobals.getLocale();
        
        String[][] timeZoneList = LocaleUtils.timeZoneLists.get(jiveLocale);
        if (timeZoneList == null) {
            String[] timeZoneIDs = LocaleUtils.timeZoneIds;
            // Now, create String[][] using the unique zones.
            timeZoneList = new String[timeZoneIDs.length][2];
            for (int i = 0; i < timeZoneList.length; i++) {
                String zoneID = timeZoneIDs[i];
                timeZoneList[i][0] = zoneID;
                timeZoneList[i][1] = LocaleUtils.getTimeZoneName(zoneID, jiveLocale);
            }
            
            // Add the new list to the map of locales to lists
            LocaleUtils.timeZoneLists.put(jiveLocale, timeZoneList);
        }
        
        return timeZoneList;
    }
    
    public static String getTimeZoneName(String zoneID, Locale locale) {
        TimeZone zone = TimeZone.getTimeZone(zoneID);
        StringBuffer buf = new StringBuffer();
        // Add in the GMT part to the name. First, figure out the offset.
        int offset = zone.getRawOffset();
        if (zone.inDaylightTime(new Date()) && zone.useDaylightTime()) {
            offset += (int) IpvsServConstants.HOUR;
        }
        
        buf.append("(");
        if (offset < 0) {
            buf.append("GMT-");
        } else {
            buf.append("GMT+");
        }
        offset = Math.abs(offset);
        int hours = offset / (int) IpvsServConstants.HOUR;
        int minutes = offset % (int) IpvsServConstants.HOUR / (int) IpvsServConstants.MINUTE;
        buf.append(hours).append(":");
        if (minutes < 10) {
            buf.append("0").append(minutes);
        } else {
            buf.append(minutes);
        }
        buf.append(") ");
        
        // Use a friendly english timezone name if the locale is en, otherwise use the timezone id
        if ("en".equals(locale.getLanguage())) {
            String name = LocaleUtils.nameMap.get(zoneID);
            if (name == null) {
                name = zoneID;
            }
            
            buf.append(name);
        } else {
            buf.append(zone.getDisplayName(true, TimeZone.LONG, locale).replace('_', ' ').replace('/', ' '));
        }
        
        return buf.toString();
    }
    
    public static ResourceBundle getResourceBundle(String baseName, Locale locale) {
        return ResourceBundle.getBundle(baseName, locale);
    }
    
    public static String getLocalizedString(String key) {
        Locale locale = IpvsServGlobals.getLocale();
        
        ResourceBundle bundle = ResourceBundle.getBundle(LocaleUtils.resourceBaseName, locale);
        
        return LocaleUtils.getLocalizedString(key, locale, null, bundle);
    }
    
    public static String getLocalizedString(String key, Locale locale) {
        ResourceBundle bundle = ResourceBundle.getBundle(LocaleUtils.resourceBaseName, locale);
        
        return LocaleUtils.getLocalizedString(key, locale, null, bundle);
    }
    
    public static String getLocalizedString(String key, List<Object> arguments) {
        Locale locale = IpvsServGlobals.getLocale();
        
        ResourceBundle bundle = ResourceBundle.getBundle(LocaleUtils.resourceBaseName, locale);
        return LocaleUtils.getLocalizedString(key, locale, arguments, bundle);
    }
    
    /*public static String getLocalizedString(String key, String pluginName) {
        return getLocalizedString(key, pluginName, null);
    }*/

    /*public static String getLocalizedString(String key, String pluginName, List arguments) {
        if (pluginName == null) {
            return getLocalizedString(key, arguments);
        }

        Locale locale = NXDConst.getLocale();
        String i18nFile = pluginName + "_i18n";

        // Retrieve classloader from pluginName.
        final XMPPServer xmppServer = XMPPServer.getInstance();
        PluginManager pluginManager = xmppServer.getPluginManager();
        Plugin plugin = pluginManager.getPlugin(pluginName);
        if (plugin == null) {
            throw new NullPointerException("Plugin could not be located: " + pluginName);
        }

        ClassLoader pluginClassLoader = pluginManager.getPluginClassloader(plugin);
        try {
            ResourceBundle bundle = ResourceBundle.getBundle(i18nFile, locale, pluginClassLoader);
            return getLocalizedString(key, locale, arguments, bundle);
        }
        catch (MissingResourceException mre) {
            Log.error(mre);
            return key;
        }
    }*/

    /* public static ResourceBundle getPluginResourceBundle(String pluginName) throws Exception {
         final Locale locale = NXDConst.getLocale();

         String i18nFile = pluginName + "_i18n";

         // Retrieve classloader from pluginName.
         final XMPPServer xmppServer = XMPPServer.getInstance();
         PluginManager pluginManager = xmppServer.getPluginManager();
         Plugin plugin = pluginManager.getPlugin(pluginName);
         if (plugin == null) {
             throw new NullPointerException("Plugin could not be located.");
         }

         ClassLoader pluginClassLoader = pluginManager.getPluginClassloader(plugin);
         return ResourceBundle.getBundle(i18nFile, locale, pluginClassLoader);
     }*/

    public static String getLocalizedString(String key, Locale locale, List<Object> arguments, ResourceBundle bundle) {
        if (key == null) {
            throw new NullPointerException("Key cannot be null");
        }
        if (locale == null) {
            locale = IpvsServGlobals.getLocale();
        }
        
        String value;
        
        // See if the bundle has a value
        try {
            // The jdk caches resource bundles on it's own, so we won't bother.
            value = bundle.getString(key);
            // perform argument substitutions
            if (arguments != null) {
                MessageFormat messageFormat = new MessageFormat("");
                messageFormat.setLocale(bundle.getLocale());
                messageFormat.applyPattern(value);
                try {
                    // This isn't fool-proof, but it's better than nothing
                    // The idea is to try and convert strings into the
                    // types of objects that the formatters expects
                    // i.e. Numbers and Dates
                    Format[] formats = messageFormat.getFormats();
                    for (int i = 0; i < formats.length; i++) {
                        Format format = formats[i];
                        if (format != null) {
                            if (format instanceof DateFormat) {
                                if (arguments.size() > i) {
                                    Object val = arguments.get(i);
                                    if (val instanceof String) {
                                        DateFormat dateFmt = (DateFormat) format;
                                        try {
                                            val = dateFmt.parse((String) val);
                                            arguments.set(i, val);
                                        } catch (ParseException ex) {
                                            MRLog.error(ex);
                                        }
                                    }
                                }
                            } else if (format instanceof NumberFormat) {
                                if (arguments.size() > i) {
                                    Object val = arguments.get(i);
                                    if (val instanceof String) {
                                        NumberFormat nbrFmt = (NumberFormat) format;
                                        try {
                                            val = nbrFmt.parse((String) val);
                                            arguments.set(i, val);
                                        } catch (ParseException ex) {
                                            MRLog.error(ex);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    value = messageFormat.format(arguments.toArray());
                } catch (IllegalArgumentException ex) {
                    MRLog.error("Unable to format resource string for key: " + key + ", argument type not supported");
                    value = "";
                }
            }
        } catch (java.util.MissingResourceException mre) {
            MRLog.warn("Missing resource for key: " + key + " in locale " + locale.toString());
            value = "";
        }
        
        return value;
    }
    
    public static String getLocalizedNumber(long number) {
        return NumberFormat.getInstance().format(number);
    }
    
    public static String getLocalizedNumber(long number, Locale locale) {
        return NumberFormat.getInstance(locale).format(number);
    }
    
    public static String getLocalizedNumber(double number) {
        return NumberFormat.getInstance().format(number);
    }
    
    public static String getLocalizedNumber(double number, Locale locale) {
        return NumberFormat.getInstance(locale).format(number);
    }
}
