using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Clock))]
public class ClockEditor : Editor
{
    bool _currently_halted = true;
    string _halt_button_text = "RESUME to Run Mode";

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        Clock clock = target as Clock;

        if (clock.mode != ClockMode.Run && GUILayout.Button("Step"))
        {
            clock.Step();
        }

        if (GUILayout.Button(_halt_button_text))
        {
            if (_currently_halted)
            {
                clock.Resume();
            }
            else
            {
                clock.Halt();
            }

            _currently_halted = clock.halted;
            _halt_button_text = _currently_halted ? "RESUME to Run Mode" : "HALT to Step Mode";
        }
    }
}