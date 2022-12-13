using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

public class LED_Bar : MonoBehaviour
{
    [SerializeField] protected List<Image> _bits = new List<Image>();
                       
    [SerializeField] protected Color _errorColor;
    [SerializeField] protected Color _tristateColor;
    [SerializeField] protected Color _offColor;
    [SerializeField] protected Color _onColor;

    [SerializeField] protected Value _value = null;

    public void SetSource(Value v)
    {
        _value = v;
    }

    protected virtual void Start()
    {
        int num_bits = this.transform.childCount;
        for (int i = 0; i < num_bits; i++)
            _bits.Add(this.transform.GetChild(i).GetComponent<Image>());
    }

    protected void Update()
    {
        if (_value.state == ValueState.Error)
        {
            for (int i = 0; i < _bits.Count; i++)
                _bits[i].color = _errorColor;
        }
        else if (_value.state == ValueState.TriState)
        {
            for (int i = 0; i < _bits.Count; i++)
                _bits[i].color = _tristateColor;
        }
        else
        {
            for (int i = 0; i < _bits.Count; i++)
            {
                bool bit_active = ((int)_value.value & (1 << i)) != 0;

                if (bit_active)
                    _bits[i].color = _onColor;
                else
                    _bits[i].color = _offColor;
            }
        }
    }
}
