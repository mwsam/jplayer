
/**
 * @file
 * Drupal behaviors for the jPlayer audio player.
 */

(function ($) {

Drupal.jPlayer = Drupal.jPlayer || { active: false };

Drupal.behaviors.jPlayer = function(context) {
  $('.jplayer', context).each(function() {
    var wrapper = $(this).parent().get(0);
    var player = this;
    var playerId = this.id;
    var playerType = $(this).attr('rel') ? 'single' : 'playlist';
    var playerPlayTime = $(wrapper).find('.jp-play-time').get(0);
    var playerTotalTime = $(wrapper).find('.jp-total-time').get(0);
    var active = 0; // The current playlist item.
    var playlist = []; // An array of DOM element links.

    // Multi-player specific code.
    if (playerType == 'playlist') {

      // Enable clicking links within the playlist.
      $(wrapper).find('.jp-playlist li a').each(function(n) {
        if ($(player).attr('rel') == '' || $(player).attr('rel') == undefined) {
          $(player).attr('rel', this.href);
        }
        playlist.push(this);
        $(this).click(function() {
          active = n;
          Drupal.jPlayer.setActive(wrapper, player, playlist, n);
          Drupal.jPlayer.play(wrapper, player);
          return false;
        });
      });

      // Enable play, pause, and stop buttons.
      $(wrapper).find('a.jp-play').click(function() {
        Drupal.jPlayer.play(wrapper, player);
        return false;
      });
      $(wrapper).find('a.jp-pause').click(function() {
        Drupal.jPlayer.pause(wrapper, player);
        return false;
      });
      $(wrapper).find('a.jp-stop').click(function() {
        Drupal.jPlayer.stop(wrapper, player);
        return false;
      });

      // Enable next and previous buttons.
      $(wrapper).find('a.jp-next').click(function() {
        active = Drupal.jPlayer.next(wrapper, player, playlist, active);
        Drupal.jPlayer.play(wrapper, player);
        return false;
      });
      $(wrapper).find('a.jp-previous').click(function() {
        active = Drupal.jPlayer.previous(wrapper, player, playlist, active);
        Drupal.jPlayer.play(wrapper, player);
        return false;
      });
    }

    // Ensure that only one jPlayer can play per page when the regular player
    // is used.
    $(wrapper).find('a.jp-play').click(function() {
      Drupal.jPlayer.pauseOthers(wrapper, player);

      // Generate the authorization URL to ping.
      var time = new Date;
      var authorize_url = Drupal.settings.basePath + 'jplayer/authorize/' + base64_encode($(player).attr('rel')) + '/' + base64_encode(parseInt(time.getTime() / 1000).toString());

      // Ping the authorization URL. We need to disable async so that this
      // command finishes before thisandler returns.
      $.ajax({
        url: authorize_url,
        async: false,
      });
      return false;
    });

    // Actually initialize the player.
    $(player).jPlayer({
      ready: function() { 
        this.element.jPlayer('setFile', this.element.attr('rel'));
        if (player.playerType == 'playlist') {
          Drupal.jPlayer.setActive(wrapper, player, playlist, active);
        }
        if (Drupal.settings.jPlayer.autoPlay && !Drupal.jPlayer.active) {
          Drupal.jPlayer.play(wrapper, player);
        }
      },
      swfPath: Drupal.settings.jPlayer.swfPath,
      volume: 50,
      oggSupport: false,
      nativeSupport: true
    })
    // Set all our custom IDs.
    .jPlayer('cssId', 'play', playerId + '-play')
    .jPlayer('cssId', 'pause', playerId + '-pause')
    .jPlayer('cssId', 'stop', playerId + '-stop')
    .jPlayer('cssId', 'loadBar', playerId + '-load-bar')
    .jPlayer('cssId', 'playBar', playerId + '-play-bar')
    .jPlayer('cssId', 'volumeMin', playerId + '-volume-min')
    .jPlayer('cssId', 'volumeMax', playerId + '-volume-max')
    .jPlayer('cssId', 'volumeBar', playerId + '-volume-bar')
    .jPlayer('cssId', 'volumeBarValue', playerId + '-volume-bar-value')
    // Register progress functions.
    .jPlayer('onProgressChange', function(loadPercent, playedPercentRelative, playedPercentAbsolute, playedTime, totalTime) {
      $(playerPlayTime).text($.jPlayer.convertTime(playedTime));
      if (totalTime != 0 && totalTime != Number.POSITIVE_INFINITY) {
        $(playerTotalTime).text($.jPlayer.convertTime(totalTime));
      }
    })
    .jPlayer('onSoundComplete', function() {
      if (playerType == 'playlist') {
        Drupal.jPlayer.next(wrapper, player, playlist, active);
      }
    });
    $.jPlayer.timeFormat.showHour = true;
  });
};

Drupal.jPlayer.setActive = function(wrapper, player, playlist, index) {
  $(wrapper).find('.jplayer_playlist_current').removeClass('jplayer_playlist_current');
  $(playlist[index]).parent().addClass('jplayer_playlist_current');
  $(player).jPlayer('setFile', playlist[index].href);
};

/**
 * Prevent multiple players from playing at once.
 */
Drupal.jPlayer.pauseOthers = function(wrapper, player) {
  if (Drupal.settings.jPlayer.pauseOthers && Drupal.jPlayer.currentPlayer != player) {
    $(Drupal.jPlayer.currentPlayer).jPlayer('pause');
  }
  Drupal.jPlayer.currentPlayer = player;
}

Drupal.jPlayer.play = function(wrapper, player) {
  Drupal.jPlayer.pauseOthers(wrapper, player);
  $(player).jPlayer('play');
  Drupal.jPlayer.active = true;
};

Drupal.jPlayer.pause = function(wrapper, player) {
  $(player).jPlayer('pause');
  Drupal.jPlayer.active = false;
};

Drupal.jPlayer.stop = function(wrapper, player) {
  $(player).jPlayer('stop');
  Drupal.jPlayer.active = false;
};

Drupal.jPlayer.next = function(wrapper, player, playlist, current) {
  var index = (current + 1 < playlist.length) ? current + 1 : 0;
  Drupal.jPlayer.setActive(wrapper, player, playlist, index);
  Drupal.jPlayer.play(wrapper, player);
  return index;
};

Drupal.jPlayer.previous = function(wrapper, player, playlist, current) {
  var index = (current - 1 >= 0) ? current - 1 : playlist.length - 1;
  Drupal.jPlayer.setActive(wrapper, player, playlist, index);
  Drupal.jPlayer.play(wrapper, player);
  return index;
};

Drupal.jPlayer.base64Encode = function(data) {
  // From http://phpjs.org/functions/base64_encode:358 where it is
  // dual licensed under GPL/MIT.
  //
  // http://kevin.vanzonneveld.net
  // +   original by: Tyler Akins (http://rumkin.com)
  // +   improved by: Bayron Guevara
  // +   improved by: Thunder.m
  // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
  // +   bugfixed by: Pellentesque Malesuada
  // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
  // -    depends on: utf8_encode
  // *     example 1: base64_encode('Kevin van Zonneveld');
  // *     returns 1: 'S2V2aW4gdmFuIFpvbm5ldmVsZA=='
  var b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  var o1, o2, o3, h1, h2, h3, h4, bits, i = 0,
      ac = 0,
      enc = "",
      tmp_arr = [];

  if (!data) {
      return data;
  }

  data = utf8_encode(data + '');

  do { // pack three octets into four hexets
      o1 = data.charCodeAt(i++);
      o2 = data.charCodeAt(i++);
      o3 = data.charCodeAt(i++);

      bits = o1 << 16 | o2 << 8 | o3;

      h1 = bits >> 18 & 0x3f;
      h2 = bits >> 12 & 0x3f;
      h3 = bits >> 6 & 0x3f;
      h4 = bits & 0x3f;

      // use hexets to index into b64, and append result to encoded string
      tmp_arr[ac++] = b64.charAt(h1) + b64.charAt(h2) + b64.charAt(h3) + b64.charAt(h4);
  } while (i < data.length);

  enc = tmp_arr.join('');

  switch (data.length % 3) {
  case 1:
      enc = enc.slice(0, -2) + '==';
      break;
  case 2:
      enc = enc.slice(0, -1) + '=';
      break;
  }

  return enc;
};

Drupal.jPlayer.utf8_encode = function(argString) {
  // From http://phpjs.org/functions/utf8_encode:577 where it is dual-licensed
  // under GPL/MIT.
  // http://kevin.vanzonneveld.net
  // +   original by: Webtoolkit.info (http://www.webtoolkit.info/)
  // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
  // +   improved by: sowberry
  // +    tweaked by: Jack
  // +   bugfixed by: Onno Marsman
  // +   improved by: Yves Sucaet
  // +   bugfixed by: Onno Marsman
  // +   bugfixed by: Ulrich
  // +   bugfixed by: Rafal Kukawski
  // *     example 1: utf8_encode('Kevin van Zonneveld');
  // *     returns 1: 'Kevin van Zonneveld'

  if (argString === null || typeof argString === "undefined") {
      return "";
  }

  var string = (argString + ''); // .replace(/\r\n/g, "\n").replace(/\r/g, "\n");
  var utftext = "",
      start, end, stringl = 0;

  start = end = 0;
  stringl = string.length;
  for (var n = 0; n < stringl; n++) {
      var c1 = string.charCodeAt(n);
      var enc = null;

      if (c1 < 128) {
          end++;
      } else if (c1 > 127 && c1 < 2048) {
          enc = String.fromCharCode((c1 >> 6) | 192) + String.fromCharCode((c1 & 63) | 128);
      } else {
          enc = String.fromCharCode((c1 >> 12) | 224) + String.fromCharCode(((c1 >> 6) & 63) | 128) + String.fromCharCode((c1 & 63) | 128);
      }
      if (enc !== null) {
          if (end > start) {
              utftext += string.slice(start, end);
          }
          utftext += enc;
          start = end = n + 1;
      }
  }

  if (end > start) {
      utftext += string.slice(start, stringl);
  }

  return utftext;
};

})(jQuery);

