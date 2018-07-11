<p>This plugin implements trigger with mono input and mono output. There are up to eight samples available to play for different note velocities.</p>

<p><b>Controls:</b></p>
<ul>
	<li><b>Bypass</b> - hot bypass switch, when turned on (led indicator is shining), the plugin does not affect the input signal.</li>
	<li><b>Working area</b> - this control allows to switch the UI from trigger mode to instrument mode.</li>
	<li><b>Pause</b> - pauses any updates of the trigger graph.</li>
	<li><b>Clear</b> - clears all graphs.</li>
	<li><b>Meters</b> - meters of the input signal (right and left channels), sidechain signal and trigger level signal.
		Corresponding buttons allow to switch on/off the metering of the corresponding signal and the visibility of the corresponding graph.
	</li>
</ul>
<p><b>'Trigger' section:</b></p>
<ul>
	<li><b>Preamp</b> - input signal amplification for the sidechain.</li>
	<li><b>Mode</b> - combo box that allows to switch different modes for sidechain.</li>
	<li><b>Source</b> - part of the input signal to use for sidechain processing.</li>
	<li><b>Active</b> - trigger activity indicator.</li>
	<li><b>Reactivity</b> - the reactivity of the sidechain.</li>
	<li><b>Attack level</b> - the minimum level of the sidechain signal that forces trigger to trigger.</li>
	<li><b>Attack time</b> - the time gap used by the trigger to prevent false note-on detection.</li>
	<li><b>Release level</b> - the maximum level (relative to <b>Attack level</b>) of the sidechain signal that forces trigger to shut down.</li>
	<li><b>Release time</b> - the time gap used by the trigger to completely turn off after the signal becomes lower than <b>Release level</b>.</li>
	<li><b>Dynamics spread</b> - this knobs allows to add some dynamics to output signal.</li>
	<li><b>Dynamics range 1</b>, <b>Dynamics range 2</b> - the bounds of the range that allows to translate trigger level into velocity.</li>
</ul>
<p><b>'Samples' section:</b></p>
<ul>
	<li><b>Sample #</b> - the selector of the current displayable/editable sample.</li>
	<li><b>Head cut</b> - the time to be cut from the beginning of the current sample.</li>
	<li><b>Tail cut</b> - the time to be cut from the end of the current sample.</li>
	<li><b>Fade in</b> - the time to be faded from the beginning of the current sample.</li>
	<li><b>Fade out</b> - the time to be faded from the end of the current sample.</li>
	<li><b>Makeup</b> - the makeup gain of the sample volume.</li>
	<li><b>Pre-delay</b> - the time delay between the MIDI note has triggered and the start of the sample's playback</li>
	<li><b>Listen</b> - the button that forces the sample playback of the selected sample</li>
</ul>
<p><b>'Sample matrix' section:</b></p>
<ul>
	<li><b>Enabled</b> - enables/disables the playback of the corresponding sample.</li>
	<li><b>Active</b> - indicates that the sample is loaded, enabled and ready for playback.</li>
	<li><b>Velocity</b> - the maximum velocity of the note the sample can trigger. Allows to set up velocity layers between different samples.</li>
	<li><b>Gain</b> - the additional gain adjust for the corresponding sample.</li>
	<li><b>Listen</b> - the button that forces the playback of the corresponding sample.</li>
	<li><b>Note on</b> - indicates that the playback event of the correponding sample has triggered.</li>
</ul>
<p><b>'Audio channel' section:</b></p>
<ul>
	<li><b>Dry</b> - the gain of the input signal passed to the audio inputs of the plugin.</li>
	<li><b>Wet</b> - the gain of the processed signal.</li>
	<li><b>Output gain</b> - the overall output gain of the plugin.</li>
</ul>