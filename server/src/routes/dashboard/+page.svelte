<script lang="ts">
  import { getModalStore } from '@skeletonlabs/skeleton';
  import type { ModalSettings, ModalComponent, ModalStore } from '@skeletonlabs/skeleton';
  import plug from '$lib/assets/plug.svg';
  import addDeviceModalComponent from '$lib/components/Modal.svelte';
  import { SlideToggle } from '@skeletonlabs/skeleton';
  import { onMount } from 'svelte';
	import type { RealtimeChannel } from '@supabase/supabase-js';

  const modalComponent: ModalComponent = { ref: addDeviceModalComponent };
  const modalStore = getModalStore();

  type formData = {
		name: string;
		macList: string[];
	};

  const modalAdd: ModalSettings = {
    type: 'component',
    component: modalComponent,
    backdropClasses: '!backdrop-blur-md',
    modalClasses: '!bg-white',
    title: 'Add Device',
    response(r : formData | undefined | boolean) {
      if (typeof(r) == 'object'){
        addDeviceMac = r.macList.join(':');
        addDeviceName = r.name;
        addDevice();
      }
      console.log(r);
    },
  };

  const modalAlert: ModalSettings = {
    type: 'alert',
    backdropClasses: '!backdrop-blur-md',
    modalClasses: '!bg-white !rounded-lg !px-8 !py-7',
    title: 'You cannot turn this device on.',
    body: 'This device has detected smoke and has been turned off automatically. If you want to turn this device on, please turn it on physically.',
    buttonTextCancel: 'Close'
  };
  
  let addDeviceMac = '';
  let addDeviceName= '';

  const statusColors = ['text-firewall-red', 'text-green-500'];
  const statusLabels = ['OFF', 'ON'];
              
  interface Device {
    id: number;
    name: string;
    mac: string;
    status: boolean;
    smoke: boolean;
    connected: boolean;
  }

  export let data;

  $: ({ user, supabase } = data);

  let devices: Device[] = [];
  let deviceSub: RealtimeChannel;

  onMount(async () => {
    const { data, error } = await supabase
      .from('devices')
      .select()
      .eq('user', user?.id)
      .order('name', { ascending: true });

    if (error) {
      console.error(error);
    } else {
      devices = data;
    }

    console.log('owo', supabase.channel('devices'));

    deviceSub = supabase
    .channel('devices')
    .on('postgres_changes',
      { event: '*', schema: 'public', table: 'devices' },
      (payload) => {
        // @ts-ignore
        if (payload.new.user != user?.id) return;

        if (payload.eventType == 'INSERT') {
          devices = [...devices, payload.new] as Device[];
        } else if (payload.eventType == 'DELETE') {
          devices = devices.filter(x => x.id != payload.old.id);
        } else if (payload.eventType == 'UPDATE') {
          const idx = devices.findIndex((x) => x.id == payload.new.id);

          if (idx != -1) {
            devices[idx] = payload.new as Device;
          } else {
            devices = [...devices, payload.new] as Device[];
          }
        }
      }
    )
    .subscribe();
  });

  async function addDevice() {
    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: addDeviceMac,
        name: addDeviceName
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }

  async function toggleDevice(device: Device) {
    const { data, error } = await supabase
      .from('actions')
      .insert({
        mac: device.mac,
        action: device.status ? 'OFF' : 'ON'
      })
      .select();
    
    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }

    devices = devices;
  }
</script>

<div class='grid grid-flow-row grid-cols-1 lg:grid-cols-2 gap-4 mx-6 my-0 overflow-y-auto'>
  <div class='flex flex-row justify-between w-full h-min lg:col-span-2 items-center'>
    <h1 class='text-xl sm:text-3xl font-kollektif'>Devices</h1>
    <button class='border border-white bg-firewall-red p-2 sm:p-4 rounded-lg w-32 sm:w-40 h-min text-sm text-white font-DM hover:border-firewall-red' on:click={() => { modalStore.trigger(modalAdd); }}>+ Add Device</button>
  </div>
  {#if devices.some((x) => x.smoke)}
    <div class='flex flex-row space-x-2 justify-left items-center text-xs font-bold py-2 s-auto rounded-lg lg:col-span-2'>
      <span class='shrink-0 bg-firewall-red text-white text-lg w-8 h-8 rounded-full flex justify-center items-center'>!</span>
      <span class='text-firewall-red sm:text-base'>WARNING: One of your devices has detected smoke and has been turned off automatically.</span>
    </div>
  {/if}
  {#each devices as device (device.mac)}
    <div class='flex flex-row flex-none w-auto h-min pl-4 pr-2 py-4 sm:p-4 sm:p-8 text-sm sm:text-base items-center bg-gray-200 rounded-lg border border-gray-200 font-DM gap-6 justify-between font-bold'>
      <img src={ plug } alt='' class='size-20 sm:size-24 md:size-32' />
      <div class='basis-1/3 h-auto w-1/4 grid grid-flow-cols justify-items-left justify-self-center'>
        <span class='text-base sm:text-lg w-full truncate'>{ device.name }</span>
        <span class='italic w-full truncate'>{ device.mac }</span>
          <span class='text-firewall-red text-xs sm:text-sm w-full truncate'>   
          {#if !device.connected}
            Device not yet connected
          {:else if device.smoke}
            Smoke detected!
          {:else}
            <br>
          {/if}
        </span>
      </div>
      <div class='flex flex-col h-full w-1/4 text-center justify-center items-center md:text-lg'>
        {#if device.connected}
        <span class='font-bold { statusColors[Number(device.status)] }'>{ statusLabels[Number(device.status)] }</span>
        <button on:click={() => {if (device.smoke) {modalStore.trigger(modalAlert)}}}>
          <SlideToggle name="slide" bind:disabled={device.smoke} bind:checked={device.status} active="bg-green-500" background='bg-firewall-red' size="md" on:click={() => toggleDevice(device)} />
        </button>
        {/if}
      </div>
    </div>
  {/each}
</div>

{#if devices.length == 0}
  <div class='fixed top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 text-gray-500 text-center'>
    <span class='text-lg sm:text-xl md:text-2xl'>No devices registered.</span>
  </div>
{/if}